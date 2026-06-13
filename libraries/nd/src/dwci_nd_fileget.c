#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"


BOOL DWCi_Ndi_response_contents(NHTTPRes *res, tagDWCi_Ndi_HTTPREQ *httpReq)
{
  int iVar1;
  char *local_20;
  
  iVar1 = NHTTP_GetBodyAll(res,&local_20);
  //return (uint)(iVar1 == (httpReq->param).list.intnum);
  return iVar1 == (httpReq->param.file.filesize);
}