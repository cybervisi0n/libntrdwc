#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

static u8 db[] = {0x00, 0xCA, 0x9A, 0x3B, 
                0x00, 0xE1, 0xF5, 0x05, 
                0x80, 0x96, 0x98, 0x00,
                0x40, 0x42, 0x0F, 0x00,
                0xA0, 0x86, 0x01, 0x00,
                0x10, 0x27, 0x00, 0x00,
                0xE8, 0x03, 0x00, 0x00,
                0x64, 0x00, 0x00, 0x00,
                0x0A, 0x00, 0x00, 0x00};


int NHTTPi_compareToken(char *param_1, char *param_2)
{
  BOOL bVar1;
  int iVar2;
  int iVar3;
  
  while( TRUE ) {
    bVar1 = FALSE;
    if (('@' < *param_2) && (*param_2 < '[')) {
      bVar1 = TRUE;
    }
    if (bVar1) {
      iVar3 = *param_2 + 0x20;
    }
    else {
      iVar3 = (int)*param_2;
    }
    bVar1 = FALSE;
    if (('@' < *param_1) && (*param_1 < '[')) {
      bVar1 = TRUE;
    }
    if (bVar1) {
      iVar2 = *param_1 + 0x20;
    }
    else {
      iVar2 = (int)*param_1;
    }
    if (iVar2 != iVar3) break;
    if ((*param_1 == '\0') || (*param_1 == ' ')) {
      return 0;
    }
    param_1 = param_1 + 1;
    param_2 = param_2 + 1;
  }
  return -1;
}


int NHTTPi_encodeUrlChar(char *param_1, char param_2)
{
  char cVar1;
  int iVar2;
  byte bVar3;
  
  if (param_2 == ' ') {
    *param_1 = '+';
    iVar2 = 1;
  } else if ((((param_2 < '0') || ('9' < param_2)) && ((param_2 < 'A' || ('Z' < param_2)))) &&
          ((param_2 < 'a' || ('z' < param_2)))) {
    bVar3 = param_2 >> 4 & 0xf;
    *param_1 = '%';
    if (bVar3 < 10) {
      cVar1 = '0';
    }
    else {
      cVar1 = '7';
    }
    param_1[1] = bVar3 + cVar1;
    if ((param_2 & 0xfU) < 10) {
      cVar1 = '0';
    }
    else {
      cVar1 = '7';
    }
    param_1[2] = (param_2 & 0xfU) + cVar1;
    iVar2 = 3;
  }
  else {
    *param_1 = param_2;
    iVar2 = 1;
  }
  return iVar2;
}


int NHTTPi_getUrlEncodedSize(char *param_1)
{
  char cVar1;
  int iVar2;
  char *pcVar3;
  
  iVar2 = 0;
  cVar1 = *param_1;
  pcVar3 = param_1 + 1;
  while (cVar1 != '\0') {
    if ((((cVar1 < '0') || ('9' < cVar1)) && ((cVar1 < 'A' || ('Z' < cVar1)))) &&
       (((cVar1 < 'a' || ('z' < cVar1)) && (cVar1 != ' ')))) {
      iVar2 = iVar2 + 3;
    }
    else {
      iVar2 = iVar2 + 1;
    }
    cVar1 = *pcVar3;
    pcVar3 = pcVar3 + 1;
  }
  return iVar2;
}


int NHTTPi_intToStr(char *str, int num)
{
  #ifdef SDK_PORT
  sprintf(str, "%d", num);
  return strlen(str);
  #endif


  BOOL bVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  u8 * puVar9;
  uint *puVar10;
  uint local_48 [10];
  
  puVar10 = local_48;
  puVar9 = db;
  iVar8 = 2;
  do {
    uVar2 = *(undefined4 *)puVar9;
    uVar4 = *(undefined4 *)(puVar9 + 4);
    uVar5 = *(undefined4 *)(puVar9 + 8);
    uVar6 = *(undefined4 *)(puVar9 + 0xc);
    puVar9 = (undefined1 *)(puVar9 + 0x10);
    *puVar10 = uVar2;
    puVar10[1] = uVar4;
    puVar10[2] = uVar5;
    puVar10[3] = uVar6;
    puVar10 = puVar10 + 4;
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  *puVar10 = *(undefined4 *)puVar9;
  iVar8 = 0;
  bVar1 = FALSE;
  for (iVar7 = 0; iVar7 < 9; iVar7 = iVar7 + 1) {
    if ((uint)num < local_48[iVar7]) {
      if (bVar1) {
        if (str != (char *)0x0) {
          str[iVar8] = '0';
        }
        iVar8 = iVar8 + 1;
      }
    } else {
      //iVar3 = _u32_div_f(num,local_48[iVar7]); //TODO
      num = num - iVar3 * local_48[iVar7];
      bVar1 = TRUE;
      if (str != (char *)0x0) {
        str[iVar8] = (char)((uint)((iVar3 + 0x30) * 0x1000000) >> 0x18);
      }
      iVar8 = iVar8 + 1;
    }
  }
  if (str != (char *)0x0) {
    str[iVar8] = (char)((num + 0x30U) * 0x1000000 >> 0x18);
  }
  return iVar8 + 1;
}

int NHTTPi_memfind(char *param_1, int param_2, char *param_3, int param_4)
{
  int iVar1;
  int iVar2;
  
  if (param_4 <= param_2) {
    for (iVar1 = 0; iVar1 < (param_2 - param_4) + 1; iVar1 = iVar1 + 1) {
      if (*param_3 == param_1[iVar1]) {
        for (iVar2 = 1; (iVar2 < param_4 && (param_1[iVar2 + iVar1] == param_3[iVar2]));
            iVar2 = iVar2 + 1) {
        }
        if (iVar2 == param_4) {
          return 0;
        }
      }
    }
  }
  return -1;
}


int NHTTPi_strToHex(char *param_1,int param_2)
{
  int iVar1;
  BOOL bVar2;
  BOOL bVar3;
  char cVar4;
  int iVar5;
  int iVar6;
  
  if (param_2 < 9) {
    if ((param_2 == 8) && ('7' < *param_1)) {
      iVar5 = -1;
    }
    else {
      iVar5 = 0;
      bVar3 = FALSE;
      for (iVar6 = 0; iVar6 < param_2; iVar6 = iVar6 + 1) {
        bVar2 = FALSE;
        if (('@' < param_1[iVar6]) && (param_1[iVar6] < '[')) {
          bVar2 = TRUE;
        }
        if (bVar2) {
          cVar4 = param_1[iVar6] + ' ';
        }
        else {
          cVar4 = param_1[iVar6];
        }
        iVar1 = (int)cVar4;
        if ((iVar1 < 0x30) || (0x39 < iVar1)) {
          if ((iVar1 < 0x61) || (0x66 < iVar1)) {
            if (bVar3) {
              if (iVar1 == 0x20) {
                return iVar5;
              }
              if (iVar1 == 0) {
                return iVar5;
              }
            }
            if ((bVar3) || (iVar1 != 0x20)) {
              return -1;
            }
          }
          else {
            iVar5 = iVar5 * 0x10 + iVar1 + -0x57;
            bVar3 = TRUE;
          }
        }
        else {
          iVar5 = iVar5 * 0x10 + iVar1 + -0x30;
          bVar3 = TRUE;
        }
      }
    }
  }
  else {
    iVar5 = -1;
  }
  return iVar5;
}


int NHTTPi_strToInt(char *str,int len)
{
  BOOL bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  if (10 < len) {
    return -1;
  }
  iVar4 = 0;
  bVar1 = FALSE;
  iVar3 = 0;
  do {
    if (len <= iVar4) {
      return iVar3;
    }
    iVar5 = (int)str[iVar4];
    if (bVar1) {
      if (iVar5 == 0x20) {
        return iVar3;
      }
      if (iVar5 == 0) {
        return iVar3;
      }
    }
    if ((bVar1) || (iVar2 = iVar3, iVar5 != 0x20)) {
      if ((iVar5 < 0x30) || (0x39 < iVar5)) {
        return -1;
      }
      bVar1 = TRUE;
      iVar2 = iVar5 + iVar3 * 10 + -0x30;
      if (iVar2 < iVar3) {
        return -1;
      }
    }
    iVar4 = iVar4 + 1;
    iVar3 = iVar2;
  } while( TRUE );
}


int NHTTPi_strtonum(char *param_1,int param_2)
{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  iVar1 = 0;
  do {
    if (param_2 == 0) {
      if (iVar2 == 0) {
        iVar1 = -1;
      }
      return iVar1;
    }
    if (((*param_1 != ' ') && ('/' < *param_1)) && (*param_1 < ':')) {
      iVar1 = (int)*param_1 + iVar1 * 10 + -0x30;
      iVar2 = iVar2 + 1;
      if (9 < iVar2) {
        return -1;
      }
    }
    param_1 = param_1 + 1;
    param_2 = param_2 + -1;
  } while( TRUE );
}