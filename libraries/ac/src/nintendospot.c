#include <nitro.h>
#include <nitroWiFi.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"

#include "dwc_memfunc.h"

typedef ushort wctype_t;

typedef struct DWCWDSWork DWCWDSWork, *PDWCWDSWork;

typedef struct tagDWCWDSData tagDWCWDSData, *PtagDWCWDSData;

typedef struct WDSBriefApInfo WDSBriefApInfo, *PWDSBriefApInfo;

typedef struct WDSApInfo WDSApInfo, *PWDSApInfo;

//TODO
typedef int undefined;

struct WDSApInfo {
    uchar ssid[32];
    uchar apnum[10];
    unsigned short hotspotid;
    uchar hotspotname[24];
    uchar wepkey[32];
    uchar channel;
    uchar encryptflag;
    uchar infoflag;
    uchar reserve[5];
    unsigned short mtu;
    unsigned short crc;
};

struct WDSBriefApInfo {
    int isvalid;
    unsigned short rssi;
    struct WDSApInfo apinfo;
};

struct DWCWDSWork {
    struct tagDWCWDSData *nspotInfoBuf;
    unsigned long long wdsScanBeginTick;
    struct WDSBriefApInfo briefapinfo[16];
    int selectedIndex;
};

typedef enum AppState {
    APP_STATE_WDSINIT=0,
    APP_STATE_WDSWAITINIT=1,
    APP_STATE_WDSSCAN=2,
    APP_STATE_WDSWAITSCAN=3,
    APP_STATE_WDSCOMPLETESCAN=4,
    APP_STATE_WDSENDSCAN=5,
    APP_STATE_WDSWAITENDSCAN=6,
    APP_STATE_WDSCOMPLETEENDSCAN=7,
    APP_STATE_WDSWAITEND=8,
    APP_STATE_WDSCOMPLETEEND=9,
    APP_STATE_ERRORENDPROCESS=10,
    APP_STATE_ERROREND=11
} AppState;

AppState gAppstate;
DWCWDSWork * gDwcWdsWork;
void * wdsSysBuf;

static void DumpWDSApInfo(void * param_1);
static undefined4 IsValidApnum(void * param_1);
static void WDS_Initialize_CB(void);
static void WDS_StartScan_CB(void);
static void WDS_EndScan_CB(void);
static void WDS_End_CB(void);
static void WDS_Error_End_CB(void);

BOOL DWC_AC_StartupGetWDSInfo( DWCWDSData* nspotInfo )
{
  BOOL bVar1;
  undefined4 uVar2;
  
  if (nspotInfo == (DWCWDSData *)0x0) {
    //OSi_Panic(_14823,0x5c,_14824);
  }
  //uVar2 = WDS_GetWorkAreaSize(nspotInfo != (DWCWDSData *)0x0);
  wdsSysBuf = (void *)DWC_Alloc(1,uVar2);
  gDwcWdsWork = (DWCWDSWork *)DWC_Alloc(1,0x790);
  bVar1 = FALSE;
  if ((wdsSysBuf != (void *)0x0) && (gDwcWdsWork != (DWCWDSWork *)0x0)) {
    bVar1 = TRUE;
  }
  if (!bVar1) {
    //OSi_Panic(_14823,0x61,_14825);
  }
  MI_CpuClear8(gDwcWdsWork,0x790);
  gDwcWdsWork->nspotInfoBuf = nspotInfo;
  gAppstate = APP_STATE_WDSINIT;
  return 1;
}

DWCWDSState DWC_AC_ProcessGetWDSInfo( void )
{
  longlong lVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  unsigned_short uVar5;
  unsigned_long_long uVar6;
  undefined8 uVar7;
  
  if ((wdsSysBuf == (void *)0x0) || (gDwcWdsWork == (DWCWDSWork *)0x0)) {
    uVar2 = 3;
  }
  else {
    uVar2 = 1;
    switch(gAppstate) {
    case APP_STATE_WDSINIT:
      gAppstate = APP_STATE_WDSWAITINIT;
      //iVar4 = WDS_Initialize(wdsSysBuf,WDS_Initialize_CB,0);
      if (iVar4 == 0) {
        //DWC_Printf(0x2000000,_14864);
      }
      else {
        //DWC_Printf(0x2000000,_14865);
        gAppstate = APP_STATE_ERROREND;
      }
      break;
    case APP_STATE_WDSWAITINIT:
      break;
    case APP_STATE_WDSSCAN:
      gAppstate = APP_STATE_WDSWAITSCAN;
      //iVar4 = WDS_StartScan(WDS_StartScan_CB);
      if (iVar4 == 0) {
        if (*(int *)((int)&gDwcWdsWork->wdsScanBeginTick + 4) == 0 &&
            *(int *)&gDwcWdsWork->wdsScanBeginTick == 0) {
          uVar6 = OS_GetTick();
          gDwcWdsWork->wdsScanBeginTick = uVar6;
        }
      }
      else {
        //DWC_Printf(0x2000000,_14866);
        gAppstate = APP_STATE_ERRORENDPROCESS;
      }
      break;
    case APP_STATE_WDSWAITSCAN:
      break;
    case APP_STATE_WDSCOMPLETESCAN:
      uVar7 = OS_GetTick();
      lVar1 = (ulonglong)((uint)uVar7 - *(uint *)&gDwcWdsWork->wdsScanBeginTick) * 0x40;
      //uVar7 = _ll_udiv((int)lVar1,
      //                 ((int)((ulonglong)uVar7 >> 0x20) -
      //                 (*(int *)((int)&gDwcWdsWork->wdsScanBeginTick + 4) +
      //                 (uint)((uint)uVar7 < *(uint *)&gDwcWdsWork->wdsScanBeginTick))) * 0x40 +
      //                 (int)((ulonglong)lVar1 >> 0x20),0x82ea,0);
      if ((int)((ulonglong)uVar7 >> 0x20) == 0 && (uint)uVar7 < 3000) {
        gAppstate = APP_STATE_WDSSCAN;
      }
      else {
        gAppstate = APP_STATE_WDSENDSCAN;
      }
      break;
    case APP_STATE_WDSENDSCAN:
      gAppstate = APP_STATE_WDSWAITENDSCAN;
      //iVar4 = WDS_EndScan(WDS_EndScan_CB);
      if (iVar4 == 0) {
        //DWC_Printf(0x2000000,_14868);
      }
      else {
        //DWC_Printf(0x2000000,_14869);
        gAppstate = APP_STATE_ERRORENDPROCESS;
      }
      break;
    case APP_STATE_WDSWAITENDSCAN:
      break;
    case APP_STATE_WDSCOMPLETEENDSCAN:
      //iVar4 = WDS_GetApInfoAll(gDwcWdsWork->briefapinfo);
      if (iVar4 != 0) {
        //DWC_Printf(0x2000000,_14870);
        gAppstate = APP_STATE_ERRORENDPROCESS;
      }
      gDwcWdsWork->selectedIndex = -1;
      uVar5 = 0;
      for (iVar4 = 0; iVar4 < 0x10; iVar4 = iVar4 + 1) {
        if ((((gDwcWdsWork->briefapinfo[iVar4].isvalid != 0) &&
             ((gDwcWdsWork->briefapinfo[iVar4].apinfo.infoflag & 2) != 0)) &&
            ((gDwcWdsWork->briefapinfo[iVar4].apinfo.encryptflag == '\0' ||
             (((gDwcWdsWork->briefapinfo[iVar4].apinfo.encryptflag == '\x01' ||
               (gDwcWdsWork->briefapinfo[iVar4].apinfo.encryptflag == '\x02')) ||
              (gDwcWdsWork->briefapinfo[iVar4].apinfo.encryptflag == '\x03')))))) &&
           (iVar3 = IsValidApnum(gDwcWdsWork->briefapinfo[iVar4].apinfo.apnum), iVar3 != 0)) {
          if (uVar5 <= gDwcWdsWork->briefapinfo[iVar4].rssi) {
            gDwcWdsWork->selectedIndex = iVar4;
            uVar5 = gDwcWdsWork->briefapinfo[iVar4].rssi;
          }
          //DWC_Printf(0x2000000,_14871,gDwcWdsWork->briefapinfo[iVar4].rssi);
          DumpWDSApInfo(&gDwcWdsWork->briefapinfo[iVar4].apinfo);
        }
      }
      gAppstate = APP_STATE_WDSWAITEND;
      //iVar4 = WDS_End(WDS_End_CB);
      if (iVar4 == 0) {
        //DWC_Printf(0x2000000,_14872);
      }
      else {
        //DWC_Printf(0x2000000,_14873);
        gAppstate = APP_STATE_ERROREND;
      }
      break;
    case APP_STATE_WDSWAITEND:
      break;
    case APP_STATE_WDSCOMPLETEEND:
      if (gDwcWdsWork->selectedIndex < 0) {
        //DWC_Printf(0x2000000,_14874);
        gAppstate = APP_STATE_ERROREND;
      }
      else {
        MI_CpuCopy8(&gDwcWdsWork->briefapinfo[gDwcWdsWork->selectedIndex].apinfo,
                    gDwcWdsWork->nspotInfoBuf,0x20);
        MI_CpuCopy8(gDwcWdsWork->briefapinfo[gDwcWdsWork->selectedIndex].apinfo.wepkey,
                    gDwcWdsWork->nspotInfoBuf->wep,0x20);
        gDwcWdsWork->nspotInfoBuf->wepMode =
             (uint)gDwcWdsWork->briefapinfo[gDwcWdsWork->selectedIndex].apinfo.encryptflag;
        MI_CpuCopy8(gDwcWdsWork->briefapinfo[gDwcWdsWork->selectedIndex].apinfo.apnum,
                    gDwcWdsWork->nspotInfoBuf->apnum,10);
        uVar2 = 2;
      }
      break;
    case APP_STATE_ERRORENDPROCESS:
      gAppstate = APP_STATE_WDSWAITEND;
      //iVar4 = WDS_End(WDS_Error_End_CB);
      if (iVar4 == 0) {
        //DWC_Printf(0x2000000,_14872);
      }
      else {
        //DWC_Printf(0x2000000,_14873);
        gAppstate = APP_STATE_ERROREND;
      }
      break;
    case APP_STATE_ERROREND:
      uVar2 = 3;
    }
  }
  return uVar2;
}

void DWC_AC_CleanupGetWDSInfo( void )
{
  undefined4 in_r3;
  
  if (wdsSysBuf != (void *)0x0) {
    //DWC_Free(1,wdsSysBuf,0,in_r3,in_r3);
    wdsSysBuf = (void *)0x0;
  }
  if (gDwcWdsWork != (DWCWDSWork *)0x0) {
    DWC_Free(1,gDwcWdsWork,0);
    gDwcWdsWork = (DWCWDSWork *)0x0;
  }
  return;
}

static void DumpWDSApInfo(void * param_1)
{
  int iVar1;
  undefined auStack_30 [10];
  undefined local_26;
  undefined local_10;
  
  //DWC_Printf(0x2000000,_14881);
  MI_CpuCopy8(param_1,auStack_30,0x20);
  local_10 = 0;
  //DWC_Printf(0x2000000,_14882,auStack_30);
  MI_CpuCopy8(param_1 + 0x20,auStack_30,10);
  local_26 = 0;
  //DWC_Printf(0x2000000,_14883,auStack_30);
  //DWC_Printf(0x2000000,_14884,*(undefined *)(param_1 + 100));
  //DWC_Printf(0x2000000,_14885,*(undefined *)(param_1 + 0x65));
  //DWC_Printf(0x2000000,_14886);
  for (iVar1 = 0; iVar1 < 0x20; iVar1 = iVar1 + 1) {
    //OS_TPrintf(&_14887,*(undefined *)(param_1 + iVar1 + 0x44));
  }
  //OS_TPrintf(&_14888);
  //DWC_Printf(0x2000000,_14881);
  return;
}

static undefined4 IsValidApnum(void * param_1)
{
  int iVar1;
  
  iVar1 = 0;
  while( TRUE ) {
    if (9 < iVar1) {
      return 1;
    }
    if ((*(byte *)(param_1 + iVar1) < 0x20) || (0x7e < *(byte *)(param_1 + iVar1))) break;
    iVar1 = iVar1 + 1;
  }
  return 0;
}

static void WDS_Initialize_CB(void)
{
  undefined4 in_r3;
  
  //DWC_Printf(0x2000000,_14897,gAppstate,in_r3,in_r3);
  gAppstate = APP_STATE_WDSSCAN;
  return;
}

static void WDS_StartScan_CB(void)
{
  gAppstate = APP_STATE_WDSCOMPLETESCAN;
  return;
}

static void WDS_EndScan_CB(void)
{
  undefined4 in_r3;
  
  //DWC_Printf(0x2000000,_14900,gAppstate,in_r3,in_r3);
  gAppstate = APP_STATE_WDSCOMPLETEENDSCAN;
  return;
}

static void WDS_End_CB(void)
{
  undefined4 in_r3;
  
  //DWC_Printf(0x2000000,_14902,gAppstate,in_r3,in_r3);
  gAppstate = APP_STATE_WDSCOMPLETEEND;
  return;
}

static void WDS_Error_End_CB(void)
{
  undefined4 in_r3;
  
  //DWC_Printf(0x2000000,_14904,gAppstate,in_r3,in_r3);
  gAppstate = APP_STATE_ERROREND;
  return;
}