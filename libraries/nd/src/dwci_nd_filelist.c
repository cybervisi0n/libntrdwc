#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

BOOL DWCi_Ndi_response_list(NHTTPRes *res, tagDWCi_Ndi_HTTPREQ *httpReq)
{
  int iVar1;
  uint uVar2;
  int iVar3;
  DWCNdFileInfo *dst;
  int iVar4;
  int iVar5;
  char *local_48;
  int local_44;
  uint local_40;
  int local_3c;
  char *local_38 [2];
  
  local_40 = 0;
  DWCi_Ndi_memclr((httpReq->param).list.filelist,(httpReq->param).list.intnum * 0xb0);
  iVar1 = NHTTP_GetHeaderField(res, "Content-Length", &local_48);
  if (((iVar1 < 1) || (*local_48 != '0')) || (local_48[1] != '\0')) {
    local_3c = NHTTP_GetBodyAll(res,local_38);
    if (0 < local_3c) {
      iVar1 = 0;
      for (iVar4 = 0; iVar4 < (httpReq->param).list.intnum; iVar4 = iVar4 + 1) {
        if (local_3c <= iVar1) {
          return 1;
        }
        dst = (httpReq->param).list.filelist + iVar4;
        iVar5 = 0;
        local_44 = 0;
        local_40 = 0;
        iVar3 = iVar1;
        for (; iVar1 < local_3c; iVar1 = iVar1 + 1) {
          if (local_38[0][iVar1] == '\r') {
            local_44 = 1;
          }
          else {
            if (local_44 == 0) {
              if (local_38[0][iVar1] == '\t') {
                switch(iVar5) {
                case 0:
                  if (0x20 < (uint)(iVar1 - iVar3)) {
                    return local_40;
                  }
                  DWCi_Ndi_memcpy(dst,local_38[0] + iVar3,iVar1 - iVar3);
                  break;
                case 1:
                  uVar2 = DWCi_Ndi_getDecodeLenBASE64(iVar1 - iVar3);
                  if (0x66 < uVar2) {
                    return local_40;
                  }
                  DWCi_Ndi_decodeBASE64((char *)dst->explain,local_38[0] + iVar3,iVar1 - iVar3);
                  dst->explain[0x32] = 0;
                  break;
                case 2:
                  if (10 < (uint)(iVar1 - iVar3)) {
                    return local_40;
                  }
                  DWCi_Ndi_memcpy(dst->param1,local_38[0] + iVar3,iVar1 - iVar3);
                  break;
                case 3:
                  if (10 < (uint)(iVar1 - iVar3)) {
                    return local_40;
                  }
                  DWCi_Ndi_memcpy(dst->param2,local_38[0] + iVar3,iVar1 - iVar3);
                  break;
                case 4:
                  if (10 < (uint)(iVar1 - iVar3)) {
                    return local_40;
                  }
                  DWCi_Ndi_memcpy(dst->param3,local_38[0] + iVar3,iVar1 - iVar3);
                  break;
                case 5:
                  if (8 < iVar1 - iVar3) {
                    return local_40;
                  }
                  uVar2 = DWCi_Ndi_strToInt(local_38[0] + iVar3,iVar1 - iVar3);
                  if ((int)uVar2 < 0) {
                    return local_40;
                  }
                  dst->size = uVar2;
                }
                iVar5 = iVar5 + 1;
                iVar3 = iVar1 + 1;
              }
            }
            else if (local_38[0][iVar1] == '\n') {
              if (iVar5 == 5) {
                if (8 < (iVar1 + -1) - iVar3) {
                  return local_40;
                }
                uVar2 = DWCi_Ndi_strToInt(local_38[0] + iVar3,(iVar1 + -1) - iVar3);
                if ((int)uVar2 < 0) {
                  return local_40;
                }
                dst->size = uVar2;
              }
              iVar1 = iVar1 + 1;
              local_40 = (uint)(4 < iVar5);
              break;
            }
            local_44 = 0;
          }
        }
      }
    }
  } else {
    local_40 = 1;
  }
  return local_40;
}