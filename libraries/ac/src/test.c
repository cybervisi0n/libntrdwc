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

typedef struct anon_struct_4_2_8b413704_for_rateSet anon_struct_4_2_8b413704_for_rateSet, *Panon_struct_4_2_8b413704_for_rateSet;

struct anon_struct_4_2_8b413704_for_rateSet {
    unsigned short basic;
    unsigned short support;
};

static undefined4 ConnectTestStart(tagACWORK * aAcWork);
static undefined4 GetIPAddress(tagACWORK * param_1);
static undefined4 ConnectTestCreate(void);
static undefined4 ConnectTestProcess(tagACWORK * param_1);
static undefined4 ConnectTestEnd(tagACWORK * param_1);
static undefined4 ConnectTestRetry(void);
static void MakeSOCConfig(tagACHOLD *aAcHold,tagACWORK *aAcWork,SOCConfig *aSocConfig);
static uint ConvAddress(byte *param_1);
static uint ConvNetMask(byte param_1);
static void CheckSetDNS(tagACWORK * param_1);


u8 DWCi_AC_ConnectTest(void)
{
  int iVar1;
  tagACWORK * myAcWork; /* iVar2 */
  int iVar3;
  
  iVar1 = DWCi_AC_GetPhase();
  myAcWork = DWCi_AC_GetMemPtr(0x10);
  //PCPORT_TODO: Remove this
  #ifdef SDK_PORT
  //Force phase 9
  iVar3 = 9;
  #else
  iVar3 = WCM_GetPhase();
  #endif
  if (iVar3 == 9) {
    switch(iVar1) {
    case 10:
      iVar1 = ConnectTestStart(myAcWork);
      break;
    case 0xb:
      iVar1 = ConnectTestRetry();
      break;
    case 0xc:
      iVar1 = GetIPAddress(myAcWork);
      break;
    case 0xd:
      iVar1 = ConnectTestCreate();
      break;
    case 0xe:
      iVar1 = ConnectTestProcess(myAcWork);
      break;
    case 0xf:
      iVar1 = ConnectTestEnd(myAcWork);
    }
  }
  else if (iVar1 == 0xb) {
    iVar1 = ConnectTestRetry();
  }
  else {
    if (iVar1 == 0xe) {
      DWC_Netcheck_Abort();
      DWC_Netcheck_Destroy();
    }
    else if (iVar1 == 0xf) {
      iVar1 = ConnectTestEnd(myAcWork);
      return iVar1;
    }
    //DWC_Printf(0x2000000,_13191);
    *(undefined *)(myAcWork + (uint)*(byte *)(myAcWork + 0xd13) * 4 + 0x444) = 2;
    iVar1 = 0xb;
  }
  return iVar1;
}

static undefined4 ConnectTestStart(tagACWORK * aAcWork)
{
  tagACHOLD * myAcHold; /* uVar1 */
  SOCConfig * mySocConfig; /* uVar2 */
  int iVar3;
  undefined4 ret;
  ret = 0;
  
  myAcHold = DWCi_AC_GetMemPtr(1);
  mySocConfig = DWCi_AC_GetMemPtr(4);
  MakeSOCConfig(myAcHold,aAcWork,mySocConfig);
  SOCL_SetYieldWait(4);
  iVar3 = SOC_Startup(mySocConfig);
  if (iVar3 == 0) {
    DWC_Printf(0x2000000, "SOC_Startup Success\n");
    ret = 0xc;
  }
  else {
    DWC_Printf(0x2000000, "SOC_Startup Error\n");
    //DWCi_AC_SetError(2);
    ret = 0x11;
  }
  return ret;
}

static undefined4 GetIPAddress(tagACWORK * param_1)
{
  longlong lVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined8 uVar4;
  
  uVar2 = SOC_GetHostID();
  if (uVar2 == 0) {
    uVar4 = OS_GetTick();
    lVar1 = (ulonglong)((uint)uVar4 - *(uint *)(param_1 + 0xcb0)) * 0x40;
    //uVar4 = _ll_udiv((int)lVar1,
    //                 ((int)((ulonglong)uVar4 >> 0x20) -
    //                 (*(int *)(param_1 + 0xcb4) + (uint)((uint)uVar4 < *(uint *)(param_1 + 0xcb0))))
    //                 * 0x40 + (int)((ulonglong)lVar1 >> 0x20),0x1ff6210,0);
    if ((int)((ulonglong)uVar4 >> 0x20) == 0 && (uint)uVar4 < 10) {
      uVar3 = 0xc;
    }
    else {
      //DWC_Printf(0x2000000,_13208);
      *(undefined *)(param_1 + (uint)*(byte *)(param_1 + 0xd13) * 4 + 0x444) = 1;
      uVar3 = 0xb;
    }
  }
  else {
    //DWC_Printf(0x2000000,_13206,uVar2 & 0xff,(uVar2 & 0xff00) >> 8,(uVar2 & 0xff0000) >> 0x10,
    //           uVar2 >> 0x18);
    CheckSetDNS(param_1);
    if (((uint)*(byte *)(param_1 + 0xd0c) << 0x1a) >> 0x1e == 1) {
      uVar3 = 0xf;
    }
    else {
      uVar3 = 0xd;
    }
  }
  return uVar3;
}

static undefined4 ConnectTestCreate(void)
{
  DWCNetcheckParam * myNetcheck;
  int iVar2;
  undefined4 ret;
  ret = 0;
  
  myNetcheck = DWCi_AC_GetMemPtr(8);
  iVar2 = DWC_Netcheck_Create(myNetcheck);
  if (iVar2 == 0) {
    ret = 0xe;
  }
  else {
    //DWC_Printf(0x2000000,_13214);
    //DWCi_AC_SetError(3);
    ret = 0x11;
  }
  return ret;
}

static undefined4 ConnectTestProcess(tagACWORK * param_1)
{
  tagACHOLD * myAcHold; /* iVar1 */
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  
  myAcHold = DWCi_AC_GetMemPtr(1);
  iVar2 = DWC_Netcheck_GetError();
  if (iVar2 == 0) {
    uVar4 = 0xe;
  }
  else {
    uVar3 = DWCi_ConvConnectAPType(*(undefined *)(param_1 + 0xd0d));
    if (*(byte *)(myAcHold + 0x15) == uVar3) {
      uVar4 = DWC_Netcheck_GetReturnCode();
      *(undefined4 *)(myAcHold + 0x10) = uVar4;
    }
    DWC_Netcheck_Destroy();
    if (iVar2 == 0xb) {
      //DWC_Printf(0x2000000,_13224,0xb);
      uVar4 = 0xf;
    }
    else {
      //DWC_Printf(0x2000000,_13223,iVar2);
      *(undefined *)(param_1 + (uint)*(byte *)(param_1 + 0xd13) * 4 + 0x444) = 1;
      uVar4 = 0xb;
    }
  }
  return uVar4;
}

static undefined4 ConnectTestEnd(tagACWORK * param_1)
{
  //DWC_Printf(0x2000000,_13227);
  //DWCi_AC_SetApType(*(undefined *)(param_1 + 0xd0d));
  return 0x10;
}

static undefined4 ConnectTestRetry(void)
{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = SOCL_CalmDown();
  if (iVar1 == 0) {
    iVar1 = SOC_Cleanup();
    if ((iVar1 == 0) || (iVar1 == -0x27)) {
      //DWC_Printf(0x2000000,_13236,iVar1);
      uVar2 = 9;
    }
    else {
      uVar2 = 0xb;
    }
  }
  else {
    //DWC_Printf(0x2000000,_13235);
    uVar2 = 0xb;
  }
  return uVar2;
}

//static void MakeSOCConfig(undefined4 *param_1,int param_2,int param_3)
static void MakeSOCConfig(tagACHOLD *aAcHold,tagACWORK *aAcWork,SOCConfig *aSocConfig)
{
  int iVar1;
  undefined4 uVar2;
  
  //MI_CpuCopy8(&DWC_AC_SOC_CONFIG,param_3,0x58,param_4,param_4);
  //*(undefined4 *)(param_3 + 4) = *param_1;
  //*(undefined4 *)(param_3 + 8) = param_1[1];

  //TODO: Remove void * cast
  aSocConfig->alloc = (void*)aAcHold->alloc;
  aSocConfig->free = (void*)aAcHold->free;
  //if (*(byte *)(param_2 + 0xd0d) < 6) {
  if(aAcWork->connectApType < 6) {
    iVar1 = DWCi_ConvConnectAPType(aAcWork->connectApType);
    //param_2 = param_2 + iVar1 * 0x100;
    tagDWCMemPage * myMemPage = &aAcWork->userInfo[iVar1];
    //if (*(char *)(param_2 + 0xc0) == '\0') {
    if(myMemPage->ap.ip[0] == 0) {
      //*(undefined4 *)(param_3 + 0xc) = 1;
      //*(undefined4 *)(param_3 + 0x10) = 0;
      //*(undefined4 *)(param_3 + 0x14) = 0;
      //*(undefined4 *)(param_3 + 0x18) = 0;
      //*(undefined4 *)(param_3 + 0x1c) = 0;
      //*(undefined4 *)(param_3 + 0x20) = 0;
      aSocConfig->flag = 1;
      aSocConfig->addr.addr = 0;
      aSocConfig->netmask.addr = 0;
      aSocConfig->router.addr = 0;
      aSocConfig->dns1.addr = 0;
      aSocConfig->dns2.addr = 0;
    }
    else {
      //*(undefined4 *)(param_3 + 0xc) = 0;
      //uVar2 = ConvAddress((void*)param_2 + 0xc0);
      //*(undefined4 *)(param_3 + 0x10) = uVar2;
      //uVar2 = ConvNetMask(*(undefined *)(param_2 + 0xd0));
      //*(undefined4 *)(param_3 + 0x14) = uVar2;
      //uVar2 = ConvAddress((void*)param_2 + 0xc4);
      //*(undefined4 *)(param_3 + 0x18) = uVar2;
      //uVar2 = ConvAddress((void*)param_2 + 200);
      //*(undefined4 *)(param_3 + 0x1c) = uVar2;
      //uVar2 = ConvAddress((void*)param_2 + 0xcc);
      //*(undefined4 *)(param_3 + 0x20) = uVar2;
      aSocConfig->flag = 0;
      aSocConfig->addr.addr = ConvAddress((u8*)myMemPage->ap.ip);
      aSocConfig->netmask.addr = ConvNetMask(myMemPage->ap.netmask);
      aSocConfig->router.addr = ConvAddress((u8*)myMemPage->ap.gateway);
      aSocConfig->dns1.addr = ConvAddress((u8*)myMemPage->ap.dns[0]);
      aSocConfig->dns2.addr = ConvAddress((u8*)myMemPage->ap.dns[1]);
    }
  }
  return;
}

static uint ConvAddress(byte *add)
{
	u32 address = 0;

	address |= ( *add ) << 24;
	add++;
	address |= ( *add ) << 16;
	add++;
	address |= ( *add ) << 8;
	add++;
	address |= *add;

	return SOC_HtoNl( address );
}

#ifndef SDK_BUILD_ARM
static uint ConvAddressNoHtonl(u8 * add) {
  u32 address = 0;

	address |= ( *add ) << 24;
	add++;
	address |= ( *add ) << 16;
	add++;
	address |= ( *add ) << 8;
	add++;
	address |= *add;
  return address;
}
#endif

static uint ConvNetMask(byte param_1)
{
  int iVar1;
  uint uVar2;
  
  uVar2 = 0xffffffff;
  for (iVar1 = 0; iVar1 < (int)(0x20 - (uint)param_1); iVar1 = iVar1 + 1) {
    uVar2 = uVar2 << 1;
  }
  return uVar2 << 0x18 | (uVar2 & 0xff00) << 8 | uVar2 >> 0x18 | uVar2 >> 8 & 0xff00;
}

static void CheckSetDNS(tagACWORK * aAcWork)
{
  int iVar1;
  SOCInAddr primaryDns;
  SOCInAddr secondaryDns;
  
  //if (*(byte *)(param_1 + 0xd0d) < 6) {
  if(aAcWork->connectApType < 6) {
    //iVar1 = DWCi_ConvConnectAPType(*(undefined *)(param_1 + 0xd0d));
    iVar1 = DWCi_ConvConnectAPType(aAcWork->connectApType);
    //param_1 = param_1 + iVar1 * 0x100;
    tagDWCMemPage * myMemPage = &aAcWork->userInfo[iVar1];
    //if ((*(char *)(param_1 + 0xc0) == '\0') &&
    //   ((uint)*(byte *)(param_1 + 0xcb) +
    //    (uint)*(byte *)(param_1 + 0xca) +
    //    (uint)*(byte *)(param_1 + 200) + (uint)*(byte *)(param_1 + 0xc9) != 0)) {
    //  local_14 = ConvAddress((void*)param_1 + 200);
    //  local_18 = ConvAddress((void*)param_1 + 0xcc);
    //  //SOC_SetResolver(&local_14,&local_18);
    //}
    if(myMemPage->ap.ip[0] == 0
    && (myMemPage->ap.dns[0][0] + myMemPage->ap.dns[0][1] +
        myMemPage->ap.dns[0][2] + myMemPage->ap.dns[0][3]) != 0) {
          #ifdef SDK_BUILD_ARM
          primaryDns.addr = ConvAddress((u8*)myMemPage->ap.dns[0]);
          secondaryDns.addr = ConvAddress((u8*)myMemPage->ap.dns[1]);
          #else
          primaryDns.addr = ConvAddressNoHtonl((u8*)myMemPage->ap.dns[0]);
          secondaryDns.addr = ConvAddressNoHtonl((u8*)myMemPage->ap.dns[1]);
          #endif
          SOC_SetResolver(&primaryDns,&secondaryDns);
        }
  }
  return;
}