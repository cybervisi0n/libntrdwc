#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"


void NHTTPi_memclr(void *dest, u32 size)
{
  MI_CpuClear8(dest,size);
  return;
}


void NHTTPi_memcpy(void *dst,void *src,int len)
{
  MI_CpuCopy8(src,dst,len);
}


int NHTTPi_strcmp(char *str1,char *str2)
{
  return STD_CompareString(str1,str2);
}


int NHTTPi_strncmp(char *str1,char *str2,int len)
{
  return STD_CompareNString(str1,str2,len);
}


int NHTTPi_strlen(char *param_1)
{
  return STD_GetStringLength(param_1);
}


int NHTTPi_stricmp(char *string1,char *string2)
{
  return NHTTPi_strnicmp(string1,string2,0xffffffff);
}


int NHTTPi_strnicmp(char *string1,char *string2,int len)
{
  BOOL bVar1;
  int iVar2;
  int iVar3;
  
  while( TRUE ) {
    if (len < 1) {
      return len;
    }
    iVar3 = (int)*string1;
    iVar2 = (int)*string2;
    if ((iVar3 == 0) || (iVar2 == 0)) break;
    bVar1 = FALSE;
    if ((0x40 < iVar2) && (iVar2 < 0x5b)) {
      bVar1 = TRUE;
    }
    if (bVar1) {
      iVar2 = iVar2 + 0x20;
    }
    bVar1 = FALSE;
    if ((0x40 < iVar3) && (iVar3 < 0x5b)) {
      bVar1 = TRUE;
    }
    if (bVar1) {
      iVar3 = iVar3 + 0x20;
    }
    if (iVar3 != iVar2) {
      return len;
    }
    len = len + -1;
    string2 = string2 + 1;
    string1 = string1 + 1;
  }
  if (iVar3 != 0) {
    return len;
  }
  if (iVar2 != 0) {
    return len;
  }
  return 0;
}