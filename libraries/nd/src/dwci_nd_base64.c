
#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"

static int decodeChar(uint param_1);
static int getNchar(char *param_1,char *param_2,int len);

static int decodeChar(uint param_1)
{
  int iVar1;
  
  if ((param_1 < 0x41) || (0x5a < param_1)) {
    if ((param_1 < 0x61) || (0x7a < param_1)) {
      if ((param_1 < 0x30) || (0x39 < param_1)) {
        if (param_1 == 0x2e) {
          iVar1 = 0x3e;
        }
        else if (param_1 == 0x2d) {
          iVar1 = 0x3f;
        }
        else {
          iVar1 = -1;
        }
      }
      else {
        iVar1 = param_1 + 4;
      }
    }
    else {
      iVar1 = param_1 - 0x47;
    }
  }
  else {
    iVar1 = param_1 - 0x41;
  }
  return iVar1;
}

static int getNchar(char *param_1,char *param_2,int len)
{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  for (iVar1 = 0; iVar1 < len; iVar1 = iVar1 + 1) {
    if (*param_2 == '\0') {
      *param_1 = '\0';
    }
    else {
      iVar2 = iVar2 + 1;
      *param_1 = *param_2;
      param_2 = param_2 + 1;
    }
    param_1 = param_1 + 1;
  }
  return iVar2;
}

int DWCi_Ndi_decodeBASE64(char *param_1, char *param_2, int param_3)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  byte local_28 [4];
  char local_24;
  char local_23;
  char local_22;
  
  iVar4 = 0;
  iVar1 = getNchar(local_28 + 3,param_2,4);
  iVar2 = 0;
  while( TRUE ) {
    if ((iVar1 < 1) || (iVar2 < 0)) {
      return iVar4;
    }
    local_28[0] = 0;
    local_28[1] = 0;
    local_28[2] = 0;
    iVar2 = decodeChar(local_28[3]);
    if (-1 < iVar2) {
      local_28[0] = (byte)((uint)(iVar2 << 0x1a) >> 0x18);
      iVar2 = decodeChar(local_24);
      if (-1 < iVar2) {
        local_28[0] = local_28[0] | (byte)(iVar2 >> 4);
        local_28[1] = (byte)((uint)(iVar2 << 0x1c) >> 0x18);
        iVar2 = decodeChar(local_23);
        if (-1 < iVar2) {
          local_28[1] = local_28[1] | (byte)(iVar2 >> 2);
          local_28[2] = (byte)((uint)(iVar2 << 0x1e) >> 0x18);
          iVar2 = decodeChar(local_22);
          if (-1 < iVar2) {
            local_28[2] = local_28[2] | (byte)iVar2;
          }
        }
      }
    }
    for (iVar3 = 0; iVar3 < iVar1 + -1; iVar3 = iVar3 + 1) {
      *param_1 = local_28[iVar3];
      param_1 = param_1 + 1;
    }
    param_2 = param_2 + iVar1;
    iVar4 = iVar4 + iVar1 + -1;
    param_3 = param_3 + -4;
    if (param_3 < 1) break;
    iVar1 = getNchar(local_28 + 3,param_2,4);
  }
  return iVar4;
}

int DWCi_Ndi_encodeBASE64(char *out,char *in,int inLen)
{
  char cVar1;
  int iVar2;
  char *pcVar3;
  int iVar4;

  byte local_26[3] = {0};
  
  iVar4 = 0;
  iVar2 = inLen;
  if (3 < inLen) {
    iVar2 = 3;
  }
  for (iVar2 = getNchar(local_26,in,iVar2); 0 < iVar2;
      iVar2 = getNchar(local_26,in,iVar2)) {
    if (inLen < 3) {
      local_26[2] = 0;
    }
    if (inLen < 2) {
      local_26[1] = 0;
    }
    *out = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-"
               [(int)(uint)local_26[0] >> 2];
    out[1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-"
                 [(local_26[0] & 3) << 4 | (int)(uint)local_26[1] >> 4];
    if (iVar2 < 2) {
      cVar1 = '*';
    }
    else {
      cVar1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-"
              [(byte)((byte)((local_26[1] & 0xf) << 2) | (byte)((int)(uint)local_26[2] >> 6))];
    }
    pcVar3 = out + 3;
    out[2] = cVar1;
    if (iVar2 < 3) {
      cVar1 = '*';
    }
    else {
      cVar1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-"[local_26[2] & 0x3f];
    }
    out = out + 4;
    *pcVar3 = cVar1;
    in = in + iVar2;
    iVar4 = iVar4 + 4;
    inLen = inLen - iVar2;
    if (inLen == 0) break;
    iVar2 = inLen;
    if (3 < inLen) {
      iVar2 = 3;
    }
  }
  *out = '\0';
  return iVar4;
}

int DWCi_Ndi_getDecodeLenBASE64(int param_1)
{
  return (param_1 >> 2) * 3;
}

int DWCi_Ndi_getEncodeLenBASE64(int param_1)
{

  

  return ((4 * param_1 / 3) + 3) & ~3;
}