#include  <nitro.h>
#include  <bm/dwc_bm_init.h>
#include  "dwc_backup.h"
#include  "dwc_local.h"

enum  tagDWCInitResult {
	DWC_INIT_OK          =      0,
	DWC_INIT_OK_ERASE    = -10002,
	DWC_INIT_OK_INIT     = -10003,
	DWC_INIT_ERROR_WRITE = -10000,
	DWC_INIT_ERROR_READ  = -10001
};

extern BOOL DWCi_AUTH_GetNewWiFiInfo( DWCWiFiInfo* wifiinfo);

static BOOL checkAp(const DWCApInfo* info);

static int init(DWCMemMap* mm);

static void initPage(DWCMemMap* mm, int page);

int DWC_BM_Init(void* work)
{
	MEMINIT*  wk = work;
	int       i;
	u16       hash;
	BOOL      c[4];
	BOOL      clear;

	SDK_ASSERTMSG( ( (int)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );
	

	if (! DWCi_BACKUPlInit(work))  return  DWC_INIT_ERROR_READ;

	MATH_CRC16InitTable(&wk->tbl);

	if (! DWCi_BACKUPlRead(&wk->mm))  return  DWC_INIT_ERROR_READ;

	MI_CpuClear8(c, sizeof(c));
	for (i = 0; i < 3; ++ i) {
		hash = MATH_CalcCRC16(&wk->tbl, &wk->mm.page[i], 0xFE);
		if ((hash == wk->mm.page[i].crc) && checkAp(&wk->mm.page[i].ap)) {
			c[i] = TRUE;
		}
	}
	hash = MATH_CalcCRC16(&wk->tbl, &wk->mm.page[3], 0xFE);
	if (hash == wk->mm.page[3].crc)  c[3] = TRUE;

	if (c[0] && c[1] && c[2] && c[3]) {
		DWCi_BACKUPlSetWiFi(wk->mm.page[0].wifi);
		return  DWC_INIT_OK;
	}

	if ((! c[0]) && (! c[1]) && (! c[2]) && (! c[3])) {
		init(&wk->mm);
		if (DWCi_BACKUPlWriteAll(wk))  return  DWC_INIT_OK;
		return  DWC_INIT_ERROR_WRITE;
	}
    

	if ( ((! c[0]) || (! c[1])) && ((! c[2]) || (! c[3])) ) {
		init(&wk->mm);
		if (DWCi_BACKUPlWriteAll(wk))  return  DWC_INIT_OK;
		return  DWC_INIT_ERROR_WRITE;
	}
	

	if ((! c[0]) && (! c[1])) {
		init(&wk->mm);
		if (DWCi_BACKUPlWriteAll(wk))  return  DWC_INIT_OK_INIT;
		return  DWC_INIT_ERROR_WRITE;
	}
	

	if (! c[0]) {
		initPage(&wk->mm, 0);
		MI_CpuCopy8(wk->mm.page[1].wifi, wk->mm.page[0].wifi, 0x0D);
		wk->mm.page[0].ap.state = wk->mm.page[1].ap.state;
	}
	else if (! c[1]) {
		initPage(&wk->mm, 1);
		MI_CpuCopy8(wk->mm.page[0].wifi, wk->mm.page[1].wifi, 0x0D);
		wk->mm.page[1].ap.state = wk->mm.page[0].ap.state;
	}
	DWCi_BACKUPlSetWiFi(wk->mm.page[0].wifi);
	if (! c[2])  initPage(&wk->mm, 2);
	if (! c[3])  MI_CpuClear16(&wk->mm.page[3], sizeof(DWCMemPage));

	clear = FALSE;
	for (i = 0; i < 3; ++ i) {
		if (c[i])  continue;
		if (! (wk->mm.page[0].ap.state & (1 << i)))  continue;
		wk->mm.page[0].ap.state &= ~(1 << i);
		clear = TRUE;
	}
	if (! DWCi_BACKUPlWriteAll(wk))  return  DWC_INIT_ERROR_WRITE;
	return  (clear ?  DWC_INIT_OK_ERASE : DWC_INIT_OK);
}

static BOOL checkAp(const DWCApInfo* info)
{
	u8   snm[4];

	if (info->setType == DWC_SETTYPE_NOT     )  return  TRUE;
	if (info->setType >  DWC_SETTYPE_RAKURAKU)  return  FALSE;

	if (! DWC_BACKUPlCheckSsid(info->ssid[0]))  return  FALSE;

	if (memcmp(info->ip, DWCi_SETTING_NONE, 4)) {

		if (! DWC_BACKUPlCheckAddress(info->gateway))  return  FALSE;

		if (info->netmask > 32)  return  FALSE;

		DWCi_BACKUPlConvMaskAddr(info->netmask, snm);
		if (! DWC_BACKUPlCheckIp(info->ip, snm))  return  FALSE;
	}

	if (memcmp(info->dns[0], DWCi_SETTING_NONE, 4)) {
		if (   (! DWC_BACKUPlCheckAddress(info->dns[0]))
			&& (! DWC_BACKUPlCheckAddress(info->dns[1])))
		{
			return  FALSE;
		}
	}

	return  TRUE;
}

static int init(DWCMemMap* mm)
{
	const u8*    adr;
	DWCWiFiInfo  info;
	int          rs;
	int          i;

	MI_CpuClear16(mm, sizeof(DWCMemMap));
	for (i = 0; i < 3; ++ i) {
		mm->page[i].ap.setType = DWC_SETTYPE_NOT;
	}

	rs  = DWCi_AUTH_GetNewWiFiInfo(&info);
	adr = DWCi_BACKUPlConvWifiInfo(&info);
	for (i = 0; i < 2; ++ i) {
		MI_CpuCopy8(adr, mm->page[i].wifi, sizeof(mm->page[i].wifi));
	}
	return  0;
}

static void initPage(DWCMemMap* mm, int page)
{
	MI_CpuClear16(&mm->page[page], sizeof(DWCMemPage));
	mm->page[page].ap.setType = DWC_SETTYPE_NOT;
}
