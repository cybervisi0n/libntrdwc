#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"


BOOL DWCi_Ndi_response_count(NHTTPRes *res, tagDWCi_Ndi_HTTPREQ *httpReq)
{
  int iVar1;
  BOOL BVar2;
  char *in_r3;
  char *local_20;
  
  local_20 = in_r3;
  iVar1 = NHTTP_GetBodyAll(res,&local_20);
  if (iVar1 < 0) {
    BVar2 = 0;
  } else {
    iVar1 = DWCi_Ndi_strToInt(local_20,iVar1);
    //*(int *)((httpReq->param).list.filelist)->name = iVar1;
    *httpReq->param.count.entrynum = iVar1;

    //if (*(int *)((httpReq->param).list.filelist)->name < 0) {
    if(*httpReq->param.count.entrynum < 0){
      BVar2 = 0;
    } else {
      BVar2 = 1;
    }
  }
  return BVar2;
}