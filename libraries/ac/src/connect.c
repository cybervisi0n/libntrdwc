#include <nitro.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"
#include "ac_private.h"

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

static undefined4 ConnectAP(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4);
static undefined4 ConnectStart(tagACWORK * aAcWork);
static undefined4 GetAuthMode(int param_1);
static uint GetConnectType(tagACWORK * aAcWork);
static undefined4 GetPowerMode(int param_1);
static BOOL GetWepKey(int param_1,/*undefined*/int param_2,byte *param_3);


u8 DWCi_AC_ConnectAP(void)
{
  int iVar1;
  void * uVar2;
  
  iVar1 = DWCi_AC_GetPhase();
  uVar2 = DWCi_AC_GetMemPtr(0x10);
  if (iVar1 == 7) {
    iVar1 = ConnectStart(uVar2);
  } else if (iVar1 == 8) {
    //iVar1 = ConnectAP((u64)uVar2);
    //PCPORT_TODO
    iVar1 = 10;
  }
  return iVar1;
}

static undefined4 ConnectStart(tagACWORK * aAcWork)
{
  int uVar1;
  int iVar2;
  int iVar3;

  #ifdef SDK_PORT
  //PCPORT_TODO
  return 8;
  #endif
  
  //iVar3 = aAcWork + 0x470 + (uint)*(byte *)(aAcWork + 0xd13) * 0xc0;
  uVar1 = GetConnectType(aAcWork);
  //*(undefined *)(aAcWork + 0xd0d) = uVar1;
  MI_CpuClear8((void*)aAcWork + 0xcb8,0x52);
  //iVar2 = GetWepKey(aAcWork,*(undefined *)(aAcWork + 0xd0d),aAcWork + 0xcb8);
  if (iVar2 == 0) {
    *(byte *)(aAcWork + 0xd0b) = *(byte *)(aAcWork + 0xd0b) & 0xf3;
    if (((int)(uint)*(ushort *)(iVar3 + 0x2c) >> 4 & 1U) == 1) {
      //*(undefined *)(aAcWork + (uint)*(byte *)(aAcWork + 0xd13) * 4 + 0x444) = 3;
      //DWC_Printf(0x2000000,"WEP_MISS %s\n",iVar3 + 0xc);
      return 9;
    }
  }
  else {
    *(byte *)(aAcWork + 0xd0b) = *(byte *)(aAcWork + 0xd0b) & 0xf3 | 4;
    if (((int)(uint)*(ushort *)(iVar3 + 0x2c) >> 4 & 1U) == 0) {
      //*(undefined *)(aAcWork + (uint)*(byte *)(aAcWork + 0xd13) * 4 + 0x444) = 3;
      //DWC_Printf(0x2000000,"WEP_MISS %s\n",iVar3 + 0xc);
      return 9;
    }
    if ((*(char *)(aAcWork + 0xd0d) == '\x06') && (*(char *)(iVar3 + 0x15) == '\0')) {
      //*(undefined *)(aAcWork + (uint)*(byte *)(aAcWork + 0xd13) * 4 + 0x444) = 3;
      //DWC_Printf(0x2000000,"WEP_MISS %s\n",iVar3 + 0xc);
      return 9;
    }
  }
  //*(undefined *)(aAcWork + 0xd15) = 0;
  //*(undefined *)(aAcWork + 0xd14) = 0;
  return 8;
}

static undefined4 ConnectAP(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)
{
  int iVar1;
  uint uVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  //undefined8 uVar6;

  #ifdef SDK_PORT
  //PCPORT_TODO: RemoveMe
  return 0x10;
  #endif
  
  iVar1 = WCM_GetPhase();
  iVar5 = param_1 + 0x470 + (uint)*(byte *)(param_1 + 0xd13) * 0xc0;
  if (iVar1 == 3) {
    uVar2 = GetPowerMode(param_1);
    *(char *)(param_1 + 0xd15) = *(char *)(param_1 + 0xd15) + '\x01';
    if (3 < *(byte *)(param_1 + 0xd15)) {
      //*(undefined *)(param_1 + 0xd15) = 0;
      //*(undefined *)(param_1 + (uint)*(byte *)(param_1 + 0xd13) * 4 + 0x444) = 1;
      return 9;
    }
    if (*(char *)(param_1 + 0xd15) != '\x01') {
      if (*(char *)(param_1 + 0xd14) == '\x01') {
        *(byte *)(param_1 + 0xd0b) = *(byte *)(param_1 + 0xd0b) & 0xf3;
      }
      else {
        if (*(char *)(param_1 + 0xd14) == '\x02') {
         // *(undefined *)(param_1 + 0xd15) = 0;
          //*(undefined *)(param_1 + (uint)*(byte *)(param_1 + 0xd13) * 4 + 0x444) = 3;
          return 9;
        }
        if (*(char *)(param_1 + 0xd14) == '\x03') {
          //*(undefined *)(param_1 + 0xd15) = 0;
          //*(undefined *)(param_1 + (uint)*(byte *)(param_1 + 0xd13) * 4 + 0x444) = 4;
          return 9;
        }
        if (*(char *)(param_1 + 0xd15) == '\x03') {
          *(byte *)(param_1 + 0xd0b) = *(byte *)(param_1 + 0xd0b) & 0xf3;
        }
      }
    }
    uVar3 = GetAuthMode(param_1);
    uVar4 = WCM_ConnectAsync((void*)iVar5,(void*)param_1 + 0xcb8,uVar2 | uVar3);
    //DWC_Printf(0x2000000,_13220,iVar5 + 0xc,uVar4,param_4);
  }
  else if (iVar1 == 9) {
    //*(undefined *)(param_1 + 0xd15) = 0;
    //uVar6 = OS_GetTick();
    //*(undefined8 *)(param_1 + 0xcb0) = uVar6;
    return 10;
  }
  return 8;
}

static uint GetConnectType(tagACWORK * aAcWork)
{
  int iVar1;
  byte *pbVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  
  //iVar5 = aAcWork + 0x470 + (uint)*(byte *)(aAcWork + 0xd13) * 0xc0;
  uVar6 = 0;
  if (((uint)*(byte *)(aAcWork + 0xd0c) << 0x18) >> 0x1e == 0) {
    iVar4 = 0;
    if (*(short *)(iVar5 + 10) == 0x20) {
      uVar6 = DWCi_AC_CheckNintendoSSID((WMBssDesc *)iVar5);
      if ((int)uVar6 < 1) {
        uVar6 = 0;
      }
      else {
        iVar4 = 1;
      }
    }
    else if (*(short *)(iVar5 + 10) == 8) {
      //uVar6 = DWCi_AC_CheckFreespot(iVar5);
      if (uVar6 == 0) {
        uVar6 = 0;
      }
      else {
        iVar4 = 1;
      }
    }
    for (iVar3 = 0; iVar3 < (int)(uint)*(byte *)(aAcWork + 0xd10); iVar3 = iVar3 + 1) {
      if ((*(ushort *)(iVar5 + 10) == (ushort)*(byte *)(aAcWork + iVar3 * 0x24 + 0x303)) &&
         (iVar1 = strncmp((char *)(iVar5 + 0xc),(char *)(aAcWork + 0x304 + iVar3 * 0x24),
                          (uint)*(ushort *)(iVar5 + 10)), iVar1 == 0)) {
        if (iVar4 == 0) {
          uVar6 = (uint)*(byte *)(aAcWork + iVar3 * 0x24 + 0x301);
        }
        else {
          pbVar2 = (byte *)(aAcWork + 0x300 + iVar3 * 0x24);
          *pbVar2 = *pbVar2 & 0xf | 0x10;
          *(byte *)(aAcWork + 0xd0c) = *(byte *)(aAcWork + 0xd0c) & 0x3f | 0x40;
        }
        iVar4 = iVar4 + 1;
      }
    }
  }
  else {
    iVar4 = 0;
    for (iVar5 = 0; iVar5 < (int)(uint)*(byte *)(aAcWork + 0xd10); iVar5 = iVar5 + 1) {
      if (((uint)*(byte *)(aAcWork + iVar5 * 0x24 + 0x300) << 0x18) >> 0x1c == 1) {
        if (iVar4 == 0) {
          pbVar2 = (byte *)(aAcWork + 0x300 + iVar5 * 0x24);
          *pbVar2 = *pbVar2 & 0xf;
          uVar6 = (uint)*(byte *)(aAcWork + iVar5 * 0x24 + 0x301);
        }
        iVar4 = iVar4 + 1;
      }
    }
    if (iVar4 == 1) {
      *(byte *)(aAcWork + 0xd0c) = *(byte *)(aAcWork + 0xd0c) & 0x3f;
    }
  }
  return uVar6 & 0xff;
}

static undefined4 GetPowerMode(int param_1)
{
  undefined4 uVar1;
  
  if ((*(byte *)(param_1 + 0xd0b) & 3) == 1) {
    uVar1 = 0x30000;
  }
  else {
    uVar1 = 0x20000;
  }
  return uVar1;
}

static undefined4 GetAuthMode(int param_1)
{
  undefined4 uVar1;
  
  if (((uint)*(byte *)(param_1 + 0xd0b) << 0x1c) >> 0x1e == 1) {
    uVar1 = 0xc0000;
  }
  else {
    uVar1 = 0x80000;
  }
  return uVar1;
}

static BOOL GetWepKey(int param_1,/*undefined*/int param_2,byte *param_3)
{
  switch(param_2) {
  case 0:
    goto LAB_00010654;
  case 1:
    goto LAB_00010650;
  case 2:
    param_1 = param_1 + 0x100;
LAB_00010650:
    param_1 = param_1 + 0x100;
LAB_00010654:
    *param_3 = (byte)(((uint)*(byte *)(param_1 + 0xe6) << 0x1e) >> 0x1e);
    MI_CpuCopy8((void*)param_1 + 0x80,param_3 + 2,0x10);
    break;
  case 3:
    goto LAB_00010680;
  case 4:
    goto LAB_0001067c;
  case 5:
    param_1 = param_1 + 0x100;
LAB_0001067c:
    param_1 = param_1 + 0x100;
LAB_00010680:
    *param_3 = 1;
    MI_CpuCopy8((void*)param_1 + 0xd1,param_3 + 2,5);
    break;
  case 6:
    *param_3 = 2;
    //DWCi_AC_GetNintendoUSBWepKey
    //          (param_1 + 0x47c + (uint)*(byte *)(param_1 + 0xd13) * 0xc0,param_3 + 2);
    break;
  case 7:
    *param_3 = 2;
    //DWCi_AC_GetNintendoShopWepKey
    //          (param_1 + 0x47c + (uint)*(byte *)(param_1 + 0xd13) * 0xc0,param_3 + 2);
    break;
  case 8:
    break;
  case 9:
  }
  return *param_3 != 0;
}


