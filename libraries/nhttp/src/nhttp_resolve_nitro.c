#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

u32 NHTTPi_resolveHostname(NHTTPReq *req)
{
  char cVar1;
  char *pcVar2;
  int iVar3;
  SOCLInAddr SVar4;
  int iVar5;
  SOCInAddr local_20 [2];
  
  cVar1 = req->pURL[req->nLenHost];
  req->pURL[req->nLenHost] = '\0';
  if (req->isSSL == 0) {
    iVar5 = 7;
  }
  else {
    iVar5 = 8;
  }
  pcVar2 = req->pURL;
  iVar3 = SOC_InetAtoN(pcVar2 + iVar5,local_20);
  if (iVar3 == 0) {
    SVar4 = SOCL_Resolve(pcVar2 + iVar5);
    if (SVar4 == 0) {
      return 0;
    }
    local_20[0].addr = SVar4 << 0x18 | (SVar4 & 0xff00) << 8 | SVar4 >> 0x18 | SVar4 >> 8 & 0xff00;
  }
  req->pURL[req->nLenHost] = cVar1;
  return local_20[0].addr;
}