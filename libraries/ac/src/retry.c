#include <nitro.h>
#include <nitroWiFi.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"

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

byte DWCi_AC_ConnectRetryAP(void)
{
  void * iVar1;
  undefined4 uVar2;
  byte bVar3;
  
  iVar1 = DWCi_AC_GetMemPtr(0x10);
  bVar3 = 9;
  uVar2 = WCM_GetPhase();
  switch(uVar2) {
  case 0:
    break;
  case 1:
    break;
  case 2:
    break;
  case 3:
    bVar3 = *(byte *)(iVar1 + 0xd0e);
    if (((uint)*(byte *)(iVar1 + 0xd0c) << 0x18) >> 0x1e == 1) {
      *(undefined *)(iVar1 + (uint)*(byte *)(iVar1 + 0xd13) * 4 + 0x444) = 0;
      bVar3 = 7;
    }
    else if ((2 < bVar3) && (bVar3 < 6)) {
      //DWCi_AC_SearchReStart(bVar3);
    }
    break;
  case 4:
    break;
  case 5:
    break;
  case 6:
    WCM_EndSearchAsync();
    break;
  case 7:
    break;
  case 8:
    break;
  case 9:
    WCM_DisconnectAsync();
    break;
  case 10:
    break;
  case 0xb:
    //DWCi_AC_SetError(0);
    bVar3 = 0x11;
    break;
  case 0xc:
    WCM_TerminateAsync();
    //DWCi_AC_SetError(4);
    bVar3 = 0x11;
  }
  return bVar3;
}


