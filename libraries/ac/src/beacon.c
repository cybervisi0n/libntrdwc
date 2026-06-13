#include <nitro.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"
#include "ac_private.h"

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

typedef struct tagDWCApInfo DWCBmApInfo;

typedef struct tagACFL ACFL;

typedef struct Elf32_Rela Elf32_Rela, *PElf32_Rela;

struct Elf32_Rela {
    dword r_offset; // location to apply the relocation action
    dword r_info; // the symbol table index and the type of relocation
    dword r_addend; // a constant addend used to compute the relocatable field value
};

typedef struct Elf32_Sym Elf32_Sym, *PElf32_Sym;

struct Elf32_Sym {
    dword st_name;
    dword st_value;
    dword st_size;
    byte st_info;
    byte st_other;
    word st_shndx;
};

typedef struct Elf32_Ehdr Elf32_Ehdr, *PElf32_Ehdr;

struct Elf32_Ehdr {
    byte e_ident_magic_num;
    char e_ident_magic_str[3];
    byte e_ident_class;
    byte e_ident_data;
    byte e_ident_version;
    byte e_ident_osabi;
    byte e_ident_abiversion;
    byte e_ident_pad[7];
    word e_type;
    word e_machine;
    dword e_version;
    dword e_entry;
    dword e_phoff;
    dword e_shoff;
    dword e_flags;
    word e_ehsize;
    word e_phentsize;
    word e_phnum;
    word e_shentsize;
    word e_shnum;
    word e_shstrndx;
};

static int AddList(int param_1,WMBssDesc * aBssDesc,undefined4 param_3,tagACWORK * aAcWork);
static uint CompareList(WMBssDesc * aBssDesc,uint aSearchListNum,tagACSL* aSearchList);
static uint CompareListDiff(WMBssDesc * aBssDesc,int param_2);
static uint iExtractRssi(uint param_1);
static void SetDataListTail(int param_1,WMBssDesc * aBssDesc,byte param_3,tagACWORK * aAcWork);
static void SortList(int param_1,int param_2,undefined4 param_3,undefined4 param_4);
static void UpDateList(int param_1,WMBssDesc * aBssDesc,byte param_3,tagACWORK * aAcWork);


void DWCi_AC_GetBeacon(WMBssDesc * bssdesc)
{
  tagACWORK * myAcWork; /* iVar1 */
  tagACHOLD * myAcHold; /* iVar2 */
  byte *pbVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  ushort uVar7;
  
  iVar6 = -1;
  myAcWork = DWCi_AC_GetMemPtr(0x10);
  myAcHold = DWCi_AC_GetMemPtr(1);
  myAcHold->findAP = 1;
  uchar phase;
  phase = DWCi_AC_GetPhase();
  if (phase == 3) {
    //uVar7 = (ushort)*(byte *)(myAcWork + 0xd11);
    uVar7 = myAcWork->searchChannel;
    //if ((*(short *)(aBssDesc + 10) == 0) || (*(char *)(aBssDesc + 0xc) == '\0')) {
    if ((bssdesc->ssidLength == 0) || (bssdesc->ssid[0] == 0)) {
      //DWCi_AC_SetStealthChannel(*(undefined2 *)(aBssDesc + 0x36));
      DWCi_AC_SetStealthChannel(bssdesc->channel);
    }
    //else if ((*(short *)(aBssDesc + 10) == 1) && (*(char *)(aBssDesc + 0xc) == ' ')) {
    else if ((bssdesc->ssidLength == 1) && (bssdesc->ssid[0] == ' ')) {
      DWCi_AC_SetStealthChannel(bssdesc->channel);
      //iVar6 = CompareList(aBssDesc,*(undefined *)(myAcWork + 0xd10),myAcWork + 0x300);
      iVar6 = CompareList(bssdesc, myAcWork->searchListNum, (void*)myAcWork->searchList);
    }
    else {
      //iVar6 = CompareList(aBssDesc,*(undefined *)(myAcWork + 0xd10),myAcWork + 0x300);
      iVar6 = CompareList(bssdesc, myAcWork->searchListNum, (void*)myAcWork->searchList);
    }
  }
  else if (phase == 4) {
    uVar7 = *(short *)(myAcWork + (uint)*(byte *)(myAcWork + 0xd0f) * 0xc0 + 0x4a6) - 1U & 0xff;
    iVar6 = CompareListDiff(bssdesc,(u64)myAcWork);
    if (-1 < iVar6) {
      pbVar3 = (byte *)(myAcWork + 0x447 + (uint)*(byte *)(myAcWork + 0xd0f) * 4);
      *pbVar3 = *pbVar3 & 0x7f | 0x80;
    }
  }
  else {
    if (phase != 5) {
      return;
    }
    uVar7 = (ushort)*(byte *)(myAcWork + 0xd11);
    //iVar6 = CompareList(aBssDesc,1,myAcWork + 0x300 + (uint)*(byte *)(myAcWork + 0xd0f) * 0x24,
    //                    (uint)*(byte *)(myAcWork + 0xd0f),param_4);
    if (-1 < iVar6) {
      pbVar3 = (byte *)(myAcWork + 0x300 + (uint)*(byte *)(myAcWork + 0xd0f) * 0x24);
      *pbVar3 = *pbVar3 & 0xf0 | 1;
    }
  }
  //DWC_Printf(0x2000000,0x10000,aBssDesc + 0xc);
  if (-1 < iVar6) {
    uVar4 = AddList(iVar6,bssdesc,uVar7,myAcWork);
    uVar5 = iExtractRssi(*(undefined2 *)(bssdesc + 2));
    //DWC_Printf(0x2000000,_13236,aBssDesc + 0xc,uVar5,*(undefined2 *)(aBssDesc + 0x36));
    //SortList(uVar4,(u64)myAcWork);
  }
  return;
}

static uint iExtractRssi(uint param_1)
{
  uint uVar1;
  
  uVar1 = (int)param_1 >> 2;
  if ((param_1 & 2) == 0) {
    uVar1 = uVar1 + 0x19;
  }
  return uVar1 & 0xff;
}

int DWCi_AC_CheckNintendoSSID(WMBssDesc * bssdesc)
{
  void * iVar1;
  int iVar2;
  int uVar3;
  
  iVar1 = DWCi_AC_GetMemPtr(0x10);
  if (((((*(byte *)(iVar1 + 0xd0c) & 0xf) == 0) || ((*(byte *)(iVar1 + 0xd0c) & 0xf) == 4)) &&
      (((int)(uint)*(ushort *)(bssdesc + 0x2c) >> 4 & 1U) == 1) )/* && */
     /*(iVar2 = DWCi_AC_CheckNintendoUsbAP(aBssDesc + 0xc), iVar2 == 1)*/) {
    uVar3 = 6;
  }
  else if ((((*(byte *)(iVar1 + 0xd0c) & 0xf) == 0) || ((*(byte *)(iVar1 + 0xd0c) & 0xf) == 5)) &&
          ((((int)(uint)*(ushort *)(bssdesc + 0x2c) >> 4 & 1U) == 1 /*&&*/
           /*(iVar1 = DWCi_AC_CheckNintendoShopAP(aBssDesc + 0xc), iVar1 == 1)*/))) {
    uVar3 = 7;
  }
  else {
    uVar3 = 0xffffffff;
  }
  return uVar3;
}

static uint CompareList(WMBssDesc * aBssDesc,uint aSearchListNum,tagACSL* aSearchList)
{
  uint uVar1;
  int iVar2;
  int iVar3;
  
  //if ((*(short *)(aBssDesc + 10) != 0x20) ||
  if (( aBssDesc->ssidLength != 0x20) ||
     (uVar1 = DWCi_AC_CheckNintendoSSID(aBssDesc), (int)uVar1 < 1)) {
    for (iVar3 = 0; iVar3 < (int)(aSearchListNum & 0xff); iVar3 = iVar3 + 1) {
      //if (((*(ushort *)(aBssDesc + 10) & 0xff) == (ushort)*(byte *)(param_3 + 3)) &&
      if (((aBssDesc->ssidLength & 0xff) == (aSearchList->length)) &&
         //(iVar2 = strncmp((char *)(aBssDesc + 0xc),(char *)(aSearchList + 4),
         (iVar2 = strncmp(aBssDesc->ssid,aSearchList->ssid,aBssDesc->ssidLength), iVar2 == 0)) {
        //return (uint)*(byte *)(aSearchList + 1);
        return aSearchList->type;
      }
      //aSearchList = aSearchList + 0x24;
      aSearchList++;
    }
    uVar1 = 0xffffffff;
  }
  return uVar1;
}

static uint CompareListDiff(WMBssDesc * aBssDesc,int param_2)
{
  uint uVar1;
  int iVar2;
  int iVar3;
  
  if ((*(short *)(aBssDesc + 10) != 0x20) ||
     (uVar1 = DWCi_AC_CheckNintendoSSID(aBssDesc), (int)uVar1 < 1)) {
    for (iVar3 = 0; iVar3 < (int)(uint)*(byte *)(param_2 + 0xd12); iVar3 = iVar3 + 1) {
      if ((*(short *)(aBssDesc + 10) == *(short *)(param_2 + iVar3 * 0xc0 + 0x47a)) &&
         (iVar2 = strncmp((char *)(aBssDesc + 0xc),(char *)(param_2 + 0x47c + iVar3 * 0xc0),
                          (uint)*(ushort *)(aBssDesc + 10)), iVar2 == 0)) {
        return (uint)*(byte *)(param_2 + iVar3 * 4 + 0x445);
      }
    }
    uVar1 = 0xffffffff;
  }
  return uVar1;
}

static int AddList(int param_1,WMBssDesc * aBssDesc,undefined4 param_3,tagACWORK * aAcWork)
{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = 0;
 // while ((iVar3 = -1, iVar2 < (int)(uint)*(byte *)(aAcWork + 0xd12) &&
  while ((iVar3 = -1, iVar2 < aAcWork->findListNum &&
         //(iVar1 = WM_IsBssidEqual((void*)(aBssDesc + 4),(void*)(aAcWork + 0x474 + iVar2 * 0xc0)), iVar3 = iVar2,
         (iVar1 = WM_IsBssidEqual((u8*)&aBssDesc->rssi, (u8*)&aAcWork->bssDesc[iVar2].rssi), iVar3 = iVar2,
         iVar1 == 0))) {
    iVar2 = iVar2 + 1;
  }
  if (iVar3 == -1) {
    SetDataListTail(param_1,aBssDesc,param_3,aAcWork);
    if (*(byte *)(aAcWork + 0xd12) < 10) {
      //*(char *)(aAcWork + 0xd12) = *(char *)(aAcWork + 0xd12) + '\x01';

      //PCPORT_TODO: RemoveMe
      #ifdef SDK_PORT
      aAcWork->findList[aAcWork->findListNum].state = 0;
      #endif

      aAcWork->findListNum = aAcWork->findListNum + 1;
    }
    iVar3 = 10;
  }
  else {
    UpDateList(iVar3,aBssDesc,param_3,aAcWork);
  }
  return iVar3;
}

static void SetDataListTail(int param_1,WMBssDesc * aBssDesc,byte param_3,tagACWORK * aAcWork)
{
  int uVar1;
  
  //*(undefined *)(aAcWork + 0x46d) = param_1;
  uVar1 = iExtractRssi(*(undefined2 *)(aBssDesc + 2));
  //*(undefined *)(aAcWork + 0x46e) = uVar1;
  *(byte *)(aAcWork + 0x46f) = *(byte *)(aAcWork + 0x46f) & 0x80 | param_3 & 0x7f;
  MI_CpuCopy32((void*)aBssDesc,(void*)aAcWork + 0xbf0,0xc0);
  return;
}

static void UpDateList(int param_1,WMBssDesc * aBssDesc,byte param_3,tagACWORK * aAcWork)
{
  uint uVar1;
  tagACFL * findList; /* iVar2 */
  
  //iVar2 = aAcWork + 0x444 + param_1 * 4;
  findList = &aAcWork->findList[param_1];
  uVar1 = iExtractRssi(*(undefined2 *)(aBssDesc + 2));
  //if (*(byte *)(findList + 2) < uVar1) {
  if (findList->rssi < uVar1) {
    //*(char *)(findList + 2) = (char)uVar1;
    findList->rssi = uVar1;
    //*(byte *)(findList + 3) = *(byte *)(findList + 3) & 0x80 | param_3 & 0x7f;
    findList->channel =  param_3;
  }
  //MI_CpuCopy32((void*)aBssDesc,(void*)aAcWork + 0x470 + param_1 * 0xc0,0xc0);
  MI_CpuCopy32((void*)aBssDesc,&aAcWork->bssDesc[param_1],sizeof(WMBssDesc));
  return;
}

static void SortList(int param_1,int param_2,undefined4 param_3,undefined4 param_4)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int auStack_e0 [4];
  int auStack_dc [192];
  undefined4 uStack_1c;
  
  iVar3 = param_2 + 0x444;
  iVar4 = param_2 + 0x470;
  iVar1 = param_1 + -1;
  uStack_1c = param_4;
  while ((iVar2 = iVar1, -1 < iVar2 &&
         (*(byte *)(iVar3 + iVar2 * 4 + 2) <= *(byte *)(iVar3 + param_1 * 4 + 2)))) {
    MI_CpuCopy32((void*)iVar3 + iVar2 * 4,auStack_e0,4);
    MI_CpuCopy32((void*)iVar3 + param_1 * 4,(void*)iVar3 + iVar2 * 4,4);
    MI_CpuCopy32(auStack_e0,(void*)iVar3 + param_1 * 4,4);
    MI_CpuCopy32((void*)iVar4 + iVar2 * 0xc0,auStack_dc,0xc0);
    MI_CpuCopy32((void*)iVar4 + param_1 * 0xc0,(void*)iVar4 + iVar2 * 0xc0,0xc0);
    MI_CpuCopy32(auStack_dc,(void*)iVar4 + param_1 * 0xc0,0xc0);
    iVar1 = iVar2 + -1;
    param_1 = iVar2;
  }
  MI_CpuClear32((void*)param_2 + 0x46c,4);
  MI_CpuClear32((void*)param_2 + 0xbf0,0xc0);
  return;
}