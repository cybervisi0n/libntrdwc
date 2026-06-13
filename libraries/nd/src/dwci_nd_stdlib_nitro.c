#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"


void DWCi_Ndi_memclr(void *dest, u32 size)
{
  MI_CpuClear8(dest,size);
  return;
}


void DWCi_Ndi_memcpy(void *dst,void *src,int len)
{
  MI_CpuCopy8(src,dst,len);
}


int DWCi_Ndi_strcmp(char *str1,char *str2)
{
  return STD_CompareString(str1,str2);
}


int DWCi_Ndi_strncmp(char *str1,char *str2,int len)
{
  return STD_CompareNString(str1,str2,len);
}


int DWCi_Ndi_strlen(char *param_1)
{
  return STD_GetStringLength(param_1);
}