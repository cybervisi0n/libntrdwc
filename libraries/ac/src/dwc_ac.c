#include <nitro.h>
#include <nitroWiFi.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"
#include "ac_private.h"

typedef unsigned char   undefined;

typedef struct anon_struct_112_3_a1bf8e08 anon_struct_112_3_a1bf8e08, *Panon_struct_112_3_a1bf8e08;

struct anon_struct_112_3_a1bf8e08 {
    unsigned short userName[4];
    unsigned short gameName[8];
    unsigned short padd1[44];
};

typedef struct anon_struct_4_2_8b413704_for_rateSet anon_struct_4_2_8b413704_for_rateSet, *Panon_struct_4_2_8b413704_for_rateSet;

struct anon_struct_4_2_8b413704_for_rateSet {
    unsigned short basic;
    unsigned short support;
};

typedef union anon_union_1_2_348f6f14_for___anon anon_union_1_2_348f6f14_for___anon, *Panon_union_1_2_348f6f14_for___anon;

typedef union anon_union_112_2_0e3908ba_for___anon anon_union_112_2_0e3908ba_for___anon, *Panon_union_112_2_0e3908ba_for___anon;

union anon_union_112_2_0e3908ba_for___anon {
    unsigned short userGameInfo[56];
    struct anon_struct_112_3_a1bf8e08 old_type;
};

union anon_union_1_2_348f6f14_for___anon {
    uchar gameNameCount_attribute;
    uchar attribute;
};

void DWCi_AC_FreeAll(void);
void  DWCi_AC_Free          ( u32 name, void *ptr, s32 size );

static undefined4 CheckDuplicate(void);
static void Free_Disused(void);

static tagACHOLD * acHold;
static tagACWORK * acWork;
static void * wcmBuffer;
static SOCConfig * socConfig;
static DWCNetcheckParam * netCheck;

BOOL DWC_AC_Create( DWCACConfig *config )
{
  DWCNetcheckParam *pDVar1;
  tagACHOLD *ptVar2;
  tagACWORK *ptVar3;
  int iVar4;
  BOOL BVar5;
  
  //if (config->alloc == (_func_void_ptr_unsigned_long_long *)0x0) {
  //  OSi_Panic(@13602,0x33,@13603);
  //}
  //if (config->free == (_func_void_unsigned_long_void_ptr_long *)0x0) {
  //  OSi_Panic(@13602,0x34,@13604);
  //}
  //if (3 < config->dmaNo) {
  //  OSi_Panic(@13602,0x35,@13605);
  //}
  //if (1 < config->powerMode) {
  //  OSi_Panic(@13602,0x36,@13606);
  //}
  //if (8 < (config->option).connectType) {
  //  OSi_Panic(@13602,0x37,@13607);
  //}
  //if (1 < (config->option).skipNetCheck) {
  //  OSi_Panic(@13602,0x38,@13608);
  //}
  //if ((config->option).connectType != '\0') {
  //  OSi_Warning(@13602,0x39,@13609,(config->option).connectType);
  //}
  //if ((config->option).skipNetCheck != '\0') {
  //  OSi_Warning(@13602,0x3a,@13610);
  //}
  acHold = (*config->alloc)(1,sizeof(tagACHOLD));
  if (acHold == (tagACHOLD *)0x0) {
    OS_Panic("Memory shortage");
  }
  MI_CpuClear32(acHold,0x24);
  ptVar2 = acHold;
  acHold->alloc = (void*)config->alloc;
  ptVar2->free = (void*)config->free;
  ptVar2->phase = '\x01';
  ptVar2->endSequence = '\x01';
  ptVar2->allocState = '\x01';
  
  acWork = (tagACWORK *)DWCi_AC_Alloc(0x10,sizeof(tagACWORK));
  if (acWork == (tagACWORK *)0x0) {
    OS_Panic("Memory shortage");
  }
  #ifdef SDK_BUILD_ARM
  wcmBuffer = (void *)DWCi_AC_Alloc(2,0x2300);
  #else
  wcmBuffer = (void *)DWCi_AC_Alloc(2,9512);
  #endif
  if (wcmBuffer == (void *)0x0) {
    OS_Panic("Memory shortage");
  }
  socConfig = (SOCConfig *)DWCi_AC_Alloc(4,sizeof(SOCConfig));
  if (socConfig == (SOCConfig *)0x0) {
    OS_Panic("Memory shortage");
  }
  netCheck = (DWCNetcheckParam *)DWCi_AC_Alloc(8,sizeof(DWCNetcheckParam));
  if (netCheck == (DWCNetcheckParam *)0x0) {
    OS_Panic("Memory shortage");
  }
  MI_CpuClear32(acWork,0xd18);
  MI_CpuClear32(wcmBuffer,0x2300);
  MI_CpuClear32(socConfig,0x58);
  MI_CpuClear32(netCheck,0xc);
  ptVar3 = acWork;
  acWork->dmaNo = config->dmaNo;
  ptVar3->powerMode = config->powerMode & 3;
  pDVar1 = netCheck;
  netCheck->alloc = (void*)config->alloc;
  pDVar1->free = (void*)config->free;
  pDVar1->bmworkarea = (void *)0x0;
  ptVar3->connectType = (config->option).connectType;
  ptVar3->skipNetCheck = (config->option).skipNetCheck;
  iVar4 = DWCi_BM_GetApInfo(ptVar3->userInfo);

  #ifndef SDK_BUILD_ARM
  //Create the SimAP in userInfo
  strcpy(acWork->userInfo[0].ap.ssid[0], "SimAP");
  #endif

  if (iVar4 == 0) {
    OS_Panic("Call DWC_BM_Init first");
  }
  #ifdef SDK_BUILD_ARM
  iVar4 = WCM_Init(wcmBuffer,0x2300);
  #else
  iVar4 = WCM_Init(wcmBuffer,12848); /* sizeof(WCMWork) */
  #endif
  if ((iVar4 == 1) || (4 < iVar4)) {
    DWC_Printf(DWC_REPORTFLAG_AC, "WCM_Init result %d\n", iVar4);
    DWCi_AC_FreeAll();
    BVar5 = 0;
  }
  else {
    BVar5 = 1;
  }
  return BVar5;
}

int  DWC_AC_Process( void )
{
  uint phase; /* uVar1 */
  tagACHOLD * myAcHold; /* ptVar2 */
  int uVar3;
  
  phase = DWCi_AC_GetPhase();
  myAcHold = acHold;
  if (acHold == (tagACHOLD *)0x0) {
    OS_Panic("DWC_AC_Create isn't called.");
    myAcHold = (tagACHOLD *)0x0;
  }
  if (phase == 1) {
    phase = DWCi_AC_Start();
  } else if (phase < 7) {
    uVar3 = OS_DisableInterrupts();
    phase = DWCi_AC_SearchAP();
    DWCi_AC_SetPhase(phase);
    OS_RestoreInterrupts(uVar3);
  } else if (phase < 9) {
    phase = DWCi_AC_ConnectAP();
  } else if (phase < 10) {
    phase = DWCi_AC_ConnectRetryAP();
  } else if (phase < 0x10) {
    phase = DWCi_AC_ConnectTest();
  } else if (phase == 0x11) {
    phase = DWCi_AC_Error();
  }
  DWCi_AC_SetPhase(phase);
  if (phase == 0x10) {
    uVar3 = CheckDuplicate();
    Free_Disused();
  }
  else if (phase == 0x12) {
    Free_Disused();
    uVar3 = -1;
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}

int  DWC_AC_GetStatus( void )
{
  uint uVar1;
  int uVar2;
  
  uVar1 = DWCi_AC_GetPhase();
  if (uVar1 < 2) {
    uVar2 = 0;
  }
  else if (uVar1 < 7) {
    uVar2 = 1;
  }
  else if (uVar1 == 9) {
    uVar2 = 4;
  }
  else if (uVar1 < 10) {
    uVar2 = 2;
  }
  else if (uVar1 == 0xb) {
    uVar2 = 4;
  }
  else if (uVar1 < 0x10) {
    uVar2 = 3;
  }
  else if (uVar1 == 0x10) {
    uVar2 = 5;
  }
  else if (uVar1 == 0x11) {
    uVar2 = 4;
  }
  else {
    uVar2 = DWCi_AC_GetResult();
  }
  return uVar2;
}

uchar DWC_AC_GetApType(void)
{
  uint uVar1;
  uchar uVar2;
  
  uVar2 = 0xff;
  uVar1 = DWCi_AC_GetPhase();
  if ((9 < uVar1) && (uVar1 < 0x11)) {
    uVar2 = acHold->type;
  }
  return uVar2;
}

BOOL DWC_AC_GetApSpotInfo( u8 *apSpotInfo )
{
  uint uVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  uVar1 = DWCi_AC_GetPhase();
  if (((9 < uVar1) && (uVar1 < 0x11)) && ((acHold->type == '\x04' || (acHold->type == '\b')))) {
    MI_CpuCopy8(acHold->apSpotInfo,apSpotInfo,10);
    uVar2 = 1;
  }
  return uVar2;
}

BOOL DWC_AC_Destroy( void )
{
	u8 phase = DWCi_AC_GetPhase();

	// 初期化状態もしくはエラー状態の場合はメモリを開放して終了する
	if( phase == 0 || phase == 18 )
	{
		DWCi_AC_FreeAll();
		return TRUE;
	}
	// それ以外の状態の場合、通信を終了する
	( void )DWCi_AC_CloseNetwork( &phase );

	// 現在のフェーズをセット
	DWCi_AC_SetPhase( phase );
	return FALSE;
}


void DWC_AC_SetSpecifyApEx( const void *ssid, const void *wep, int wepMode, const char *apSpotInfo, int overrideType )
{
  u8 bVar1;
  void * iVar2;
  
  iVar2 = DWCi_AC_GetMemPtr(1);
  bVar1 = FALSE;
  if ((-1 < overrideType) && (overrideType < 0xff)) {
    bVar1 = TRUE;
  }
  if (!bVar1) {
    OS_Panic("Invalid argument 'overrideType'");
  }
  if (apSpotInfo == 0) {
    MI_CpuClear8(iVar2 + 0x18,10);
  }
  else {
    MI_CpuCopy8(apSpotInfo,iVar2 + 0x18,10);
  }
  *(char *)(iVar2 + 0x22) = (char)overrideType;
  DWC_AC_SetSpecifyAp(ssid,wep,wepMode);
  return;
}


void DWC_AC_SetSpecifyAp( const void *ssid, const void *wep, int wepMode )
{
  BOOL bVar1;
  tagACWORK *myAcWork;
  undefined1 *puVar2;
  int iVar3;
  undefined4 uVar4;
  
  myAcWork = (tagACWORK *)DWCi_AC_GetMemPtr(0x10);
  bVar1 = FALSE;
  if ((wepMode < 4) && (-1 < wepMode)) {
    bVar1 = TRUE;
  }
  if (!bVar1) {
    //OSi_Panic(@13602,0x145,@13735);
  }
  if (ssid == (void *)0x0) {
    //OSi_Panic(@13602,0x147,@13736);
  }
  myAcWork->connectType = myAcWork->connectType | 1;
  //myAcWork->field_0xd0c = myAcWork->field_0xd0c & 0xf0 | 1;
  
  MI_CpuClear8(myAcWork,0xf0);
  for (iVar3 = 0; (iVar3 < 0x20 && (*(char *)(ssid + iVar3) != '\0')); iVar3++) {
    myAcWork->userInfo[0].ap.ssid[0][iVar3] = *(uchar *)(ssid + iVar3);
  }
  if ((wep == (void *)0x0) || (wepMode == 0)) {
    //puVar2 = &myAcWork->userInfo[0].ap.field_0xe6;
    //*puVar2 = *puVar2 & 0xfc;

  }
  else {
    if (wepMode == 1) {
      uVar4 = 5;
    }
    else if (wepMode == 2) {
      uVar4 = 0xd;
    }
    else {
      uVar4 = 0x10;
    }
    MI_CpuCopy8(wep,myAcWork->userInfo[0].ap.wep,uVar4);
    //puVar2 = &myAcWork->userInfo[0].ap.field_0xe6;
    //*puVar2 = *puVar2 & 0xfc | (byte)wepMode & 3;
  }
  return;
}

BOOL DWC_AC_CheckWiFiStation( const void *ssid, u16 len )
{
  BOOL ret;
  
  if (len == 0x20) {
    ret = DWCi_AC_CheckNintendoShopAP((const char *)ssid);
  }
  else {
    ret = 0;
  }
  return ret;
}

void DWCi_AC_InsertApInfo(int no, DWCBmApInfo *info)
{
  u8 bVar1;
  void * iVar2;
  
  iVar2 = DWCi_AC_GetMemPtr(0x10);
  bVar1 = FALSE;
  if ((-1 < no) && (no < 3)) {
    bVar1 = TRUE;
  }
  if (!bVar1) {
    //OSi_Panic(_13602,0x187,_13760);
  }
  if (info == 0) {
    //OSi_Panic(_13602,0x188,_13761);
  }
  MI_CpuCopy32(info,iVar2 + no * 0x100,0xf0);
  return;
}

void * DWCi_AC_Alloc(u32 name, s32 size)
{
  tagACHOLD * myAcHold;
  void * memory;
  
  myAcHold = (tagACHOLD *)DWCi_AC_GetMemPtr(1);
  if( (myAcHold->allocState & name) == 0 ) {
    myAcHold->allocState = myAcHold->allocState | (u8)name;
    memory = myAcHold->alloc(name, size);
  } else {
    memory = NULL;
  }
  return memory;
}

void DWCi_AC_Free(u32 name, void *ptr, s32 size)
{
  tagACHOLD *myAcHold;
  
  myAcHold = (tagACHOLD *)DWCi_AC_GetMemPtr(1);
  if ((myAcHold != (tagACHOLD *)0x0) && ((myAcHold->allocState & name) != 0)) {
    myAcHold->allocState = myAcHold->allocState & ~(byte)name;
    (*myAcHold->free)(name,ptr,size);
    if (name == 1) {
      acHold = (tagACHOLD *)0x0;
    }
  }
  return;
}

void DWCi_AC_FreeAll(void)
{
  tagACHOLD *myAcHold;
  void *ptr;
  
  myAcHold = (tagACHOLD *)DWCi_AC_GetMemPtr(1);
  if (myAcHold != (tagACHOLD *)0x0) {
    if ((myAcHold->allocState & 0x10) != 0) {
      ptr = (void *)DWCi_AC_GetMemPtr(0x10);
      myAcHold->allocState = myAcHold->allocState & 0xef;
      (*myAcHold->free)(0x10,ptr,0xd18);
    }
    if ((myAcHold->allocState & 8) != 0) {
      ptr = (void *)DWCi_AC_GetMemPtr(8);
      myAcHold->allocState = myAcHold->allocState & 0xf7;
      (*myAcHold->free)(8,ptr,0xc);
    }
    if ((myAcHold->allocState & 4) != 0) {
      ptr = (void *)DWCi_AC_GetMemPtr(4);
      myAcHold->allocState = myAcHold->allocState & 0xfb;
      (*myAcHold->free)(4,ptr,0x58);
    }
    if ((myAcHold->allocState & 2) != 0) {
      ptr = (void *)DWCi_AC_GetMemPtr(2);
      myAcHold->allocState = myAcHold->allocState & 0xfd;
      (*myAcHold->free)(2,ptr,0x2300);
    }
    if ((myAcHold->allocState & 1) != 0) {
      myAcHold->allocState = myAcHold->allocState & 0xfe;
      (*myAcHold->free)(1,myAcHold,0x24);
      acHold = (tagACHOLD *)0x0;
    }
  }
  return;
}

void * DWCi_AC_GetMemPtr( u32 name )
{
	if     (( name & ALLOC_NAME_HOLD ) != 0 )       return acHold;
	else if(( name & ALLOC_NAME_WCM_BUFFER ) != 0 ) return wcmBuffer;
	else if(( name & ALLOC_NAME_SOC_CONFIG ) != 0 ) return socConfig;
	else if(( name & ALLOC_NAME_NET_CHECK ) != 0 )  return netCheck;
	else if(( name & ALLOC_NAME_WORK ) != 0 )       return acWork;
	return NULL;
}

void DWCi_AC_SetPhase(u8 phase)
{
  undefined uVar1;
  tagACHOLD * myAcHold; /* iVar2 */
  tagACWORK * myAcWork; /* iVar3 */
  
  myAcHold = DWCi_AC_GetMemPtr(1);
  myAcWork = DWCi_AC_GetMemPtr(0x10);
  //*(char *)(iVar2 + 9) = (char)param_1;
  myAcHold->phase = phase;
  if (((phase < 0x10) && 
       //(*(byte *)(iVar2 + 0x16) < aPhase)) &&
       (myAcHold->endSequence < phase)) &&
      ((myAcHold->endSequence) = phase, 7 < phase)) {
    //uVar1 = DWCi_ConvConnectAPType(*(undefined *)(iVar3 + 0xd0d));
    uVar1 = DWCi_ConvConnectAPType(myAcWork->connectApType);
    //*(undefined *)(iVar2 + 0x15) = uVar1;
    myAcHold->endType = uVar1;
    myAcHold->endState =
         //*(undefined *)(iVar3 + (uint)*(byte *)(iVar3 + 0xd13) * 4 + 0x444);
         myAcWork->findList[myAcWork->connectNo].state;
  }
  return;
}

uchar DWCi_AC_GetPhase(void)
{
  uchar uVar1;
  
  if (acHold == (tagACHOLD *)0x0) {
    uVar1 = '\0';
  }
  else {
    uVar1 = acHold->phase;
  }
  return uVar1;
}

void DWCi_AC_SetError(int error)
{
  uchar uVar1;
  tagACHOLD *myAcHold;
  
  myAcHold = (tagACHOLD *)DWCi_AC_GetMemPtr(1);
  myAcHold->error = error;
  uVar1 = DWCi_AC_GetPhase();
  myAcHold->phaseError = uVar1;
  return;
}

int DWCi_AC_GetError(void)
{
  return acHold->error;
}

void DWCi_AC_SetApType(undefined param_1)
{
  uchar uVar1;
  void * iVar2;
  
  if (acHold->overrideType == '\0') {
    uVar1 = DWCi_ConvConnectAPType(param_1);
  }
  else {
    uVar1 = acHold->overrideType;
  }
  acHold->type = uVar1;
  iVar2 = WCM_GetApEssid(0);
  if (iVar2 != 0) {
    DC_InvalidateRange(iVar2,0x20);
    //DWCi_AC_GetPostalCode(iVar2,acHold->apSpotInfo);
  }
  iVar2 = 0;
  while( 1 ) {
    if (9 < iVar2) {
      return;
    }
    if ((acHold->apSpotInfo[(int)iVar2] < ' ') || ('~' < acHold->apSpotInfo[(int)iVar2])) break;
    iVar2 = iVar2 + 1;
  }
  MI_CpuClear8(acHold->apSpotInfo,10);
  return;
}

u8 DWCi_ConvConnectAPType(u8 type)
{
  if (2 < type) {
    type = type - 3 & 0xff;
  }
  return type;
}

void Free_Disused(void)
{
  DWCi_AC_Free(8,netCheck,0xc);
  DWCi_AC_Free(0x10,acWork,0xd18);
  return;
}

undefined4 CheckDuplicate(void)
{
  byte bVar1;
  WMBssDesc *pWVar2;
  tagACWORK *ptVar3;
  int iVar4;
  uint uVar5;
  
  ptVar3 = acWork;
  bVar1 = acWork->connectNo;
  pWVar2 = acWork->bssDesc;
  if (acWork->connectApType < 6) {
    for (uVar5 = 0; uVar5 < ptVar3->findListNum; uVar5 = uVar5 + 1 & 0xff) {
      if (((uVar5 != ptVar3->connectNo) && (ptVar3->findList[uVar5].type < 6)) &&
         (iVar4 = strncmp((char *)pWVar2[bVar1].bssid,(char *)ptVar3->bssDesc[uVar5].bssid,
                          (uint)ptVar3->bssDesc[uVar5].ssidLength), iVar4 == 0)) {
        return 2;
      }
    }
  }
  return 1;
}