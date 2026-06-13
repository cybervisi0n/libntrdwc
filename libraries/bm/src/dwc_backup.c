#include  <nitro.h>
#include  "dwc_backup.h"
#include  "dwc_local.h"
#include <base/dwc_report.h>

enum {
	RES_NONE,
	RES_SUCCESS,
	RES_FAILURE
};

const u8  DWCi_SETTING_NONE[4] = { 0x00, 0x00, 0x00, 0x00 };
#define  WIP_FLAG    0x01
#define  EER_FLAG    0x20

typedef struct  tagMEMSWI {
	DWCMemPage      mem;
	u8              work[0x100];
	MATHCRC16Table  tbl;
}  MEMSWI;

static u8   Work[32]  ATTRIBUTE_ALIGN(32);
                        
static u32  Address;
static vu8  Result;
static u8   Wifi[0x0E]; 
static u8   Comm;       

static u8	ChipType[2];
static u32  getAddress();
static BOOL  readNvram(u32 address, u32 size, void* buf);
static void  writeNvram(u32 address, u16 size, const void* data);
static BOOL  verify(const void* src, u32 address, u32 size, void* work);
static BOOL  writeDisable();
static s32  waitWip();
static void  cbNvram(PXIFifoTag tag, u32 data, BOOL err);

static volatile BOOL	nv_cb_occurred;
static volatile u16		nv_result;
static void Callback_NVRAM( PXIFifoTag tag, u32 data, BOOL err );

#define SR_WIP 0x01
#define SR_WEN 0x02
#define SR_EER 0x20

enum NvramCommState{
	COMM_READY = 0,
	COMM_RD,
	COMM_WE,
	COMM_WR,
	COMM_RDSR_WE,
	COMM_RDSR_WR,
	COMM_SRST,
	COMM_WRDI
}NvramCommState;

#ifdef SDK_PORT
static FILE * s_SIM_dwcbackup_fptr = NULL;
static const char * s_SIM_DWCBACKUP_FILEPATH = "firmware.bin";
#endif

BOOL DWCi_BM_GetApInfo(DWCMemPage* buf)
{
	SDK_ASSERTMSG( ( (int)buf & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", buf );
	return  (readNvram(Address, (sizeof(DWCMemPage) * 3), buf)) ? TRUE : FALSE;
}

void DWCi_BM_GetWiFiInfo(DWCWiFiInfo* buf)
{
	MI_CpuCopy8(&Wifi[ 0], &buf->attestedUserId, 6);
	buf->attestedUserId &= 0x07FFFFFFFFFF;
	MI_CpuCopy8(&Wifi[ 5], &buf->notAttestedId, 6);
	buf->notAttestedId >>= 3;
	buf->notAttestedId  &= 0x07FFFFFFFFFF;
	MI_CpuCopy8(&Wifi[10], &buf->pass, 2);
	buf->pass >>= 6;
	buf->pass  &= 0x03FF;
	MI_CpuCopy8(&Wifi[12], &buf->randomHistory, 2);
}

BOOL DWCi_BM_SetWiFiInfo(const DWCWiFiInfo* info, void* work)
{
	MEMSWI*  wk  = work;
	int      i;
	u32      adr = Address;
	u32      times;

	SDK_ASSERTMSG( ( (int)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );

	(void)DWCi_BACKUPlConvWifiInfo(info);

	MATH_CRC16InitTable(&wk->tbl);

	for (i = 0; i < 2; ++ i, adr += sizeof(DWCMemPage)) {
		if (! readNvram(adr, sizeof(DWCMemPage), &wk->mem)) {
			OS_Panic("DWCi_BM_SetWiFiInfo : readNvram failed\n");
			return  FALSE;
		}
		MI_CpuCopy8(Wifi, wk->mem.wifi, sizeof(Wifi));
		wk->mem.crc = MATH_CalcCRC16(&wk->tbl, &wk->mem, 0xFE);
		times       = 0;
		while (TRUE) {
			writeNvram(adr, sizeof(DWCMemPage), &wk->mem);
			if (verify(&wk->mem, adr, sizeof(DWCMemPage), wk->work))
				break;
		}
	}
	return  (writeDisable() ?  TRUE : FALSE);
}

BOOL DWCi_BACKUPlInit(void* work)
{
	u16*  wk = work;

	SDK_ASSERTMSG( ( (int)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );

	if (! readNvram(0x20, 32, wk))  return  FALSE;
	Address = (u32)(wk[0] * 8 - 0x400);
	return  TRUE;
}

BOOL DWCi_BACKUPlRead(DWCMemMap* mem)
{
	return  (readNvram(Address, sizeof(DWCMemMap), mem)) ?  TRUE : FALSE;
}

BOOL DWCi_BACKUPlWritePage(const DWCMemMap* data, const BOOL* page, void* work)
{
	int  i;
	u32  adr = Address;
	u32  times;

	SDK_ASSERTMSG( ( (int)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );

	for (i = 0; i < 4; ++ i, adr += sizeof(DWCMemPage)) {
		if (! page[i])  continue;
			times = 0;
			while (TRUE) {

				writeNvram(adr, sizeof(DWCMemPage), &data->page[i]);

				if (verify(&data->page[i], adr, sizeof(DWCMemPage), work)) {
					break;
				}
			}
	}
	return  (writeDisable() ?  TRUE : FALSE);
}

BOOL DWCi_BACKUPlWriteAll(MEMINIT* work)
{
	int  i;
	u32  adr = Address;
	u32  times;

	SDK_ASSERTMSG( ( (int)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );

	for (i = 0; i < 4; ++ i, adr += sizeof(DWCMemPage)) {
		work->mm.page[i].crc = MATH_CalcCRC16(&work->tbl, &work->mm.page[i]
		                                    , 0xFE);
		times = 0;
		while (TRUE) {
			writeNvram(adr, sizeof(DWCMemPage), &work->mm.page[i]);

			if (verify(&work->mm.page[i], adr, sizeof(DWCMemPage), work->work))
				break;
		}
	}
	return  (writeDisable() ?  TRUE : FALSE);
}

BOOL DWCi_BACKUPlWriteChild(DWCMemMap* data, void* work)
{
	MEMWRITEC*  wk  = work;
	int         i;
	u32         adr = Address;
	u32         times;

	SDK_ASSERTMSG( ( (int)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );

	if (! Address)  return  FALSE;

	MATH_CRC16InitTable(&wk->tbl);

	for (i = 0; i < 4; ++ i, adr += sizeof(DWCMemPage)) {
		data->page[i].crc = MATH_CalcCRC16(&wk->tbl, &data->page[i], 0xFE);
		times = 0;
		while (TRUE) {

			writeNvram(adr, sizeof(DWCMemPage), &data->page[i]);

			if (verify(&data->page[i], adr, sizeof(DWCMemPage), wk->work)) {
				break;
			}
		}
	}
	return  (writeDisable() ?  TRUE : FALSE);
}

void DWCi_BACKUPlSetWiFi(const u8* wifi)
{
	MI_CpuCopy8(wifi, Wifi, sizeof(Wifi));
}

u8 DWCi_BACKUPlConvMaskCidr(const u8* mask)
{
	int  snm;
	int  i, j;

	for (i = 0, snm = 0; i < 4; ++ i) {
		for (j = 0; j < 8; ++ j) {
			if ((mask[i] >> j) & 1)  ++ snm;
		}
	}
	return  (u8)snm;
}

void DWCi_BACKUPlConvMaskAddr(int mask, u8* buf)
{
	int  i;
	u32  snm = (0xFFFFFFFF >> mask) ^ 0xFFFFFFFF;

	for (i = 0; i < 4; ++ i) {
		buf[i] = (u8)(snm >> (24 - (8 * i)));
	}
}

BOOL DWC_BACKUPlCheckSsid(const u8* ssid)
{
	int  i;

	for (i = 0; i < 0x20; ++ i) {
		if (ssid[i])  return  TRUE;
	}
	return  FALSE;
}

BOOL DWC_BACKUPlCheckIp(const u8* ip, const u8* mask)
{
	u32  ipa;
	u32  snm;

	if (! DWC_BACKUPlCheckAddress(ip))  return  FALSE;
	MI_CpuCopy8(ip  , &ipa, sizeof(u32));
	MI_CpuCopy8(mask, &snm, sizeof(u32));
	if ((ipa |   snm ) == ~1)  return  FALSE;
	if ((ipa & (~snm)) ==  0)  return  FALSE;
	return  TRUE;
}

BOOL DWC_BACKUPlCheckAddress(const u8* address)
{
	if (address[0] == 127)  return  FALSE;
	if (address[0] <    1)  return  FALSE;
	if (address[0] >  223)  return  FALSE;
	return  TRUE;
}

#ifndef SDK_PORT
#define  INIT_NVRAM()                                                         \
{                                                                             \
	while (! PXI_IsCallbackReady(PXI_FIFO_TAG_NVRAM, PXI_PROC_ARM7))  ;       \
	PXI_SetFifoRecvCallback(PXI_FIFO_TAG_NVRAM, Callback_NVRAM);              \
}
#else
#define INIT_NVRAM()
#endif

static BOOL NVRAMm_ExecuteCommand( int nv_state, u32 adr, u16 size, u8 *srcp )
{
    OSTick	start;
	BOOL	nv_sending	 = FALSE;
	u8		*nvram_srp	 = Work;
	
	while( 1 ) {
		if( !nv_sending ) {
			
			nv_cb_occurred	= FALSE;
			
			switch( nv_state ) {
			  case COMM_RD:
			  #ifdef SDK_PORT
			  	nv_sending = TRUE;
			  	if(s_SIM_dwcbackup_fptr == NULL) {
					s_SIM_dwcbackup_fptr = fopen(s_SIM_DWCBACKUP_FILEPATH, "rb+");
				}

				fseek(s_SIM_dwcbackup_fptr, adr, SEEK_SET);
				fread(srcp, 1, size, s_SIM_dwcbackup_fptr);
				cbNvram(PXI_FIFO_TAG_NVRAM, 0, 0);
			  #else
				nv_sending	= SPI_NvramReadDataBytes( adr, size, srcp );
			  #endif
				break;
				
			  case COMM_WE:
			  #ifdef SDK_PORT
			  	if(s_SIM_dwcbackup_fptr != NULL) {
					fclose(s_SIM_dwcbackup_fptr);
					s_SIM_dwcbackup_fptr = NULL;
				}
				s_SIM_dwcbackup_fptr = fopen(s_SIM_DWCBACKUP_FILEPATH, "rb+");
				nv_sending = TRUE;
				cbNvram(PXI_FIFO_TAG_NVRAM, 0, 0);
			  #else
				nv_sending	= SPI_NvramWriteEnable();
			  #endif
				break;
				
			  case COMM_WR:
			  #ifdef SDK_PORT
			  	if(s_SIM_dwcbackup_fptr == NULL) {
					s_SIM_dwcbackup_fptr = fopen(s_SIM_DWCBACKUP_FILEPATH, "rb+");
				}
				fseek(s_SIM_dwcbackup_fptr, adr, SEEK_SET);
				fwrite(srcp, 1, size, s_SIM_dwcbackup_fptr);
			  	nv_sending = TRUE;
				cbNvram(PXI_FIFO_TAG_NVRAM, 0, 0);
			  #else
				nv_sending	= SPI_NvramPageWrite( adr, size , srcp );
			  #endif
				start		= OS_GetTick();
				break;
				
			  case COMM_RDSR_WE:
			  case COMM_RDSR_WR:
			  #ifdef SDK_PORT
			  	nv_sending = TRUE;
				*nvram_srp = 0x02;
				cbNvram(PXI_FIFO_TAG_NVRAM, 0, 0);
			  #else
				nv_sending	= SPI_NvramReadStatusRegister( nvram_srp );
			  #endif
				break;
				
			  case COMM_SRST:
			  #ifdef SDK_PORT
			  	if(s_SIM_dwcbackup_fptr != NULL) {
					fclose(s_SIM_dwcbackup_fptr);
					s_SIM_dwcbackup_fptr = NULL;
				}
				s_SIM_dwcbackup_fptr = fopen(s_SIM_DWCBACKUP_FILEPATH, "rb+");
				nv_sending = TRUE;
				cbNvram(PXI_FIFO_TAG_NVRAM, 0, 0);
			  #else
				nv_sending	= SPI_NvramSoftwareReset();
			  #endif
				break;
			
			  case COMM_WRDI:
			  #ifdef SDK_PORT
			  	if(s_SIM_dwcbackup_fptr != NULL) {
					fclose(s_SIM_dwcbackup_fptr);
					s_SIM_dwcbackup_fptr = NULL;
				}
				s_SIM_dwcbackup_fptr = fopen(s_SIM_DWCBACKUP_FILEPATH, "rb");
				nv_sending = TRUE;
				cbNvram(PXI_FIFO_TAG_NVRAM, 0, 0);
			  #else
			  	nv_sending =  SPI_NvramWriteDisable();
			  #endif
			  	break;
			}
		#ifdef SDK_PORT
		nv_cb_occurred = TRUE;
		#endif

		}else {
			if( nv_cb_occurred == TRUE ) {
				
				nv_sending = FALSE;
				
				if( nv_result == SPI_PXI_RESULT_SUCCESS ) {
					switch( nv_state ) {
					  case COMM_RD:
						return TRUE;
						
					  case COMM_WE:
						nv_state = COMM_RDSR_WE;
						break;
						
					  case COMM_WR:
						nv_state = COMM_RDSR_WR;
						break;
						
					  case COMM_RDSR_WE:
					  case COMM_RDSR_WR:
						
						DC_InvalidateRange( nvram_srp, 1 );
						
						if( nv_state == COMM_RDSR_WE ) {
							if( ( *nvram_srp & SR_WEN ) ) {
								nv_state = COMM_WR;
							}else {
								DWC_Printf( DWC_REPORTFLAG_BM, "	NVRAM ERR: Write Enable Invalid.\n");
								return FALSE;
							}
						}else {
							if( ( *nvram_srp & SR_WIP ) == 0 ) {
								return TRUE;
							}else {
								if(	  ( *nvram_srp & SR_EER )			
								   || ( OS_TicksToMilliSeconds( OS_GetTick() - start ) > 4000 ) ) {
									DWC_Printf( DWC_REPORTFLAG_BM,  "	NVRAM SR : %02x\n", *nvram_srp );
									nv_state = COMM_SRST;
								}else {
									SVC_WaitByLoop( 0x4000 );
								}
							}
						}
						break;
						
					  case COMM_SRST:
						DWC_Printf( DWC_REPORTFLAG_BM, "	NVRAM ERR: PageErase Timeout and SoftReset.\n");
						return FALSE;
					
					  case COMM_WRDI:
					  	return TRUE;
					}
				}else {
					DWC_Printf( DWC_REPORTFLAG_BM, "	NVRAM ERR: NVRAM PXI command failed.\n");
					return FALSE;
				}
			}
		}
	}
}

static BOOL readNvram(u32 address, u32 size, void* buf)
{
	DC_InvalidateRange(buf, size);
	INIT_NVRAM();

	while(1) {
		if(NVRAMm_ExecuteCommand(COMM_RD, address, (u16)size, buf) == TRUE)
			break;
		SVC_WaitByLoop(0x40000);
	}
	
	DC_InvalidateRange(buf, size);
	return  TRUE;
}

static void writeNvram(u32 address, u16 size, const void* data)
{
	u32  cnt   = 0;

	INIT_NVRAM();

	DC_StoreRange((void*)data, size);

	while(1) {
		if(NVRAMm_ExecuteCommand(COMM_WE, address, (u16)size, (u8 *)data) == TRUE)
			break;
		SVC_WaitByLoop(0x40000);
	}

	return;
}

static BOOL verify(const void* src, u32 address, u32 size, void* work)
{
	SDK_ASSERTMSG( ( (int)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );

	if (! readNvram(address, size, work))  return  FALSE;

	if (! memcmp(src, work, size)) {
		return  TRUE;
	}
	#ifdef  CS_DEBUG
	{
		OS_Printf("err : verify  - %x\n", address);
	}
	#endif
	return  FALSE;
}

static BOOL writeDisable()
{
	INIT_NVRAM();

	while(1) {
		if(NVRAMm_ExecuteCommand(COMM_WRDI, 0, 0, NULL) == TRUE)
			break;
		SVC_WaitByLoop(0x40000);
	}
	return  TRUE;
}

static void cbNvram(PXIFifoTag tag, u32 data, BOOL err)
{
	#pragma unused(tag)

	u32  com;
	u32  res;

	if (err)  goto  _error;

	res = data & 0x00FF;
	if (res != SPI_PXI_RESULT_SUCCESS)  goto  _error;

	com = (data & 0x7F00) >> 8;
	if (com != Comm)  goto  _error;
	Result = RES_SUCCESS;
	return;

_error :
	Result = RES_FAILURE;
	#ifdef  CS_DEBUG
	{
		OS_Printf("err : cbNvram  - %x %x\n", data, err);
	}
	#endif
}

static void Callback_NVRAM( PXIFifoTag tag, u32 data, BOOL err )
{
	#pragma unused(tag)
	
	u16 command		= (u16)( ( ( data & SPI_PXI_DATA_MASK ) & 0x7f00 ) >> 8 );
	
	nv_result		= (u16)( data & 0x00ff );
	nv_cb_occurred	= TRUE;
	
	if( err ) {
		nv_result = 0x00ff;
	}
	
	switch(command){
	  case SPI_PXI_COMMAND_NVRAM_READ:
		break;
	  case SPI_PXI_COMMAND_NVRAM_WREN:
		break;
	  case SPI_PXI_COMMAND_NVRAM_PW:
		break;
	  case SPI_PXI_COMMAND_NVRAM_RDSR:
		break;
	  case SPI_PXI_COMMAND_NVRAM_WRDI:
		break;
	  case SPI_PXI_COMMAND_NVRAM_PE:
		break;
	  case SPI_PXI_COMMAND_NVRAM_SR:
		break;
	  default:
		break;
	}
	if( nv_result != SPI_PXI_RESULT_SUCCESS ) {
	}
}

const u8* DWCi_BACKUPlConvWifiInfo(const DWCWiFiInfo* info)
{
	u64  tmp = info->notAttestedId;

	MI_CpuCopy8(&info->attestedUserId, &Wifi[0], 5);
	Wifi[ 5] = (u8)(  ((info->attestedUserId >> 40) & 0x07)
		           | ((tmp & 0x1F) << 3));
	tmp >>= 5;
	MI_CpuCopy8(&tmp, &Wifi[6], 4);
	Wifi[10] = (u8)(((tmp >> 32) & 0x3F) | ((info->pass & 0x03) << 6));
	Wifi[11] = (u8)((info->pass >> 2) & 0xFF);
	MI_CpuCopy8(&info->randomHistory, &Wifi[12], 2);

	return  Wifi;
}

const void* DWCi_BACKUPlGetWifi()
{
	return  Wifi;
}
