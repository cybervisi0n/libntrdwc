#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

extern NHTTPAlloc NHTTPi_alloc;
extern NHTTPFree NHTTPi_free;
extern NHTTPError NHTTPi_error;
extern int NHTTPi_idRequest;
extern int NHTTPi_isOpened;

extern tagNHTTPi_REQLIST * NHTTPi_reqQueue;
extern tagNHTTPi_REQLIST * NHTTPi_reqCurrent;


static void deleteStrList(tagNHTTPi_DATALIST *param_1);


static void deleteStrList(tagNHTTPi_DATALIST *param_1)
{
  tagNHTTPi_DATALIST *ptVar1;
  
  while (param_1 != (tagNHTTPi_DATALIST *)0x0) {
    if (param_1 == param_1->prev) {
      (*NHTTPi_free)(param_1);
      param_1 = (tagNHTTPi_DATALIST *)0x0;
    }
    else {
      ptVar1 = param_1->prev->prev;
      (*NHTTPi_free)(param_1->prev);
      param_1->prev = ptVar1;
    }
  }
  return;
}


BOOL NHTTP_CancelRequestAsync(int id)
{
  int iVar1;
  BOOL BVar2;
  
  BVar2 = 0;
  iVar1 = NHTTPi_isOpened;
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_request.c", 0x1c5, "Failed assertion NHTTPi_isOpened");
    iVar1 = 0;
  }
  NHTTPi_lockReqList();
  //if (((NHTTPi_reqCurrent != 0) && (*(int *)(NHTTPi_reqCurrent + 8) == id)) &&
  //   (*(int *)(*(int *)(NHTTPi_reqCurrent + 0xc) + 4) == 0)) {
  //  *(undefined4 *)(*(int *)(NHTTPi_reqCurrent + 0xc) + 4) = 1;
  //  NHTTPi_SocCancel(*(undefined4 *)(NHTTPi_reqCurrent + 0xc),
  //                   *(undefined4 *)(NHTTPi_reqCurrent + 0x10));
  //  BVar2 = 1;
  //}
  if (BVar2 == 0) {
    //BVar2 = NHTTPi_freeReqQueue(id);
  }
  NHTTPi_unlockReqList();
  return BVar2;
}


NHTTPReq* NHTTP_CreateRequest( char* url, NHTTPReqMethod method, char* buf, u32 len, NHTTPReqCallback callback, void* param )
{
  BOOL bVar1;
  char cVar2;
  _NHTTPRes *p_Var3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  CPSSslConnection *p_Var7;
  NHTTPReq *pNVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  uint local_40;
  int local_38;
  
  pNVar8 = (NHTTPReq *)0x0;
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_request.c", 0x4e, "Failed assertion NHTTPi_isOpened");
  }
  if (url == NULL) {
    OSi_Panic("NHTTP_request.c", 0x4f, "Failed assertion url");
  }
  if (buf == NULL) {
    OSi_Panic("NHTTP_request.c", 0x50, "Failed assertion buf");
  }
  if (callback == NULL) {
    OSi_Panic("NHTTP_request.c", 0x51, "Failed assertion callback");
  }
  if (((method == 0) || (method == 1)) || (method == 2)) {
    if (len == 0) {
      NHTTPi_error = 0xb;
    }
    else {
      pNVar8 = (NHTTPReq *)(*NHTTPi_alloc)(sizeof(NHTTPReq),4);
      if (pNVar8 == (NHTTPReq *)0x0) {
        NHTTPi_error = 1;
      }
      else {
        NHTTPi_memclr(pNVar8,sizeof(NHTTPReq));
        p_Var3 = (NHTTPRes *)(*NHTTPi_alloc)(sizeof(NHTTPRes),4);
        pNVar8->response = p_Var3;
        if (pNVar8->response == (_NHTTPRes *)0x0) {
          NHTTPi_error = 1;
        }
        else {
          NHTTPi_memclr(pNVar8->response, sizeof(NHTTPRes));
          pNVar8->response->pRecvBuf = buf;
          pNVar8->response->nLenRecvBuf = len;
          iVar4 = NHTTPi_strlen(url);
          if (iVar4 < 8) {
            NHTTPi_error = 4;
          }
          else {
            pNVar8->port = 0x50;
            iVar10 = 7;
            iVar5 = NHTTPi_strnicmp(url, "http://", 7);
            if (iVar5 != 0) {
              iVar5 = NHTTPi_strnicmp(url, "https://", 8);
              if (iVar5 != 0) {
                NHTTPi_error = 4;
                goto LAB_00010740;
              }
              pNVar8->isSSL = 1;
              pNVar8->port = 443;
              iVar10 = 8;
            }
            iVar4 = iVar4 - iVar10;
            if (0 < iVar4) {
              iVar5 = 0;
              local_38 = 0;
              iVar9 = 0;
LAB_000103ac:
              if ((iVar4 <= iVar5) || (url[iVar5 + iVar10] == '/')) goto LAB_000103c0;
              if (iVar9 == 2) {
                iVar9 = 1;
              }
              else if (iVar9 == 1) {
                cVar2 = NHTTPi_strToHex(url + iVar5 + -1 + iVar10,2);
                iVar9 = 0;
                if (cVar2 < '\0') {
                  NHTTPi_error = 4;
                  goto LAB_00010740;
                }
                if (cVar2 == '/') {
                  local_38 = local_38 + -1;
LAB_000103c0:
                  if (iVar9 == 0) {
                    pcVar6 = (char *)(*NHTTPi_alloc)(iVar10 + iVar4 + local_38 * -2 + 1,4);
                    pNVar8->pURL = pcVar6;
                    if (pNVar8->pURL == (char *)0x0) {
                      NHTTPi_error = 1;
                    } else {
                      NHTTPi_memcpy(pNVar8->pURL,url,iVar10);
                      iVar11 = 0;
                      iVar9 = 0;
                      bVar1 = FALSE;
                      for (iVar5 = 0; iVar5 < iVar4; iVar5 = iVar5 + 1) {
                        if (iVar9 == 2) {
                          iVar9 = 1;
                        } else if (iVar9 == 1) {
                          cVar2 = NHTTPi_strToHex(url + iVar5 + -1 + iVar10,2);
                          iVar9 = 0;
                          pNVar8->pURL[iVar10 + iVar11 + -1] = cVar2;
                          if (cVar2 == '/') {
                            bVar1 = TRUE;
                          }
                        } else {
                          if (url[iVar5 + iVar10] == '/') {
                            bVar1 = TRUE;
                          }
                          if ((bVar1) || (url[iVar5 + iVar10] != '%')) {
                            pNVar8->pURL[iVar10 + iVar11] = url[iVar5 + iVar10];
                          }
                          else {
                            iVar9 = 2;
                          }
                          iVar11 = iVar11 + 1;
                        }
                      }
                      pNVar8->pURL[iVar10 + iVar11] = '\0';
                      pcVar6 = pNVar8->pURL;
                      for (iVar4 = 0; iVar4 < iVar11; iVar4 = iVar4 + 1) {
                        if ((pcVar6[iVar4 + iVar10] == '/') || (pcVar6[iVar4 + iVar10] == ':')) {
                          pNVar8->nLenHost = iVar4 + iVar10;
                          break;
                        }
                      }
                      if (iVar4 == iVar11) {
                        pNVar8->nLenHost = iVar4 + iVar10;
                        pNVar8->nLenHostDesc = pNVar8->nLenHost;
                      }
                      else if (pcVar6[iVar4 + iVar10] == '/') {
                        pNVar8->nLenHostDesc = pNVar8->nLenHost;
                      }
                      else if (pcVar6[iVar4 + iVar10] == ':') {
                        for (; iVar4 < iVar11; iVar4 = iVar4 + 1) {
                          if (pcVar6[iVar4 + iVar10] == '/') {
                            pNVar8->nLenHostDesc = iVar4 + iVar10;
                            break;
                          }
                        }
                        if (iVar4 == iVar11) {
                          pNVar8->nLenHostDesc = iVar4 + iVar10;
                        }
                        else {
                          local_40 = NHTTPi_strtonum(pNVar8->pURL + pNVar8->nLenHost + 1,
                                                     pNVar8->nLenHostDesc - (pNVar8->nLenHost + 1));
                          if ((int)local_40 < 0) {
                            local_40 = pNVar8->port;
                          }
                          else if (0xffff < (int)local_40) {
                            NHTTPi_error = 4;
                            goto LAB_00010740;
                          }
                          pNVar8->port = local_40 & 0xffff;
                        }
                      }
                      if (pNVar8->isSSL == 0) {
LAB_00010710:
                        NHTTPi_memcpy(pNVar8->tagPost, "--t9Sf4yfjf1RtvDu3AA", 0x14);
                        pNVar8->param = param;
                        pNVar8->method = method;
                        pNVar8->pCallback = callback;
                        return pNVar8;
                      }
                      iVar4 = pNVar8->nLenHost - iVar10;
                      p_Var7 = (CPSSslConnection *)(*NHTTPi_alloc)(sizeof(CPSSslConnection),4);
                      pNVar8->sslConn = p_Var7;
                      if (pNVar8->sslConn == NULL) {
                        NHTTPi_error = 1;
                      } else {
                        NHTTPi_memclr(pNVar8->sslConn,sizeof(CPSSslConnection));
                        pcVar6 = (char *)(*NHTTPi_alloc)(iVar4 + 1,4);
                        pNVar8->sslConn->server_name = pcVar6;
                        if (pNVar8->sslConn->server_name != NULL) {
                          NHTTPi_memcpy(pNVar8->sslConn->server_name,pNVar8->pURL + iVar10,iVar4);
                          pNVar8->sslConn->server_name[iVar4] = '\0';
                          goto LAB_00010710;
                        }
                        NHTTPi_error = 1;
                      }
                    }
                  }
                  else {
                    NHTTPi_error = 4;
                  }
                  goto LAB_00010740;
                }
              }
              else if (url[iVar5 + iVar10] == '%') {
                iVar9 = 2;
                local_38 = local_38 + 1;
              }
              iVar5 = iVar5 + 1;
              goto LAB_000103ac;
            }
            NHTTPi_error = 4;
          }
        }
      }
    }
  } else {
    NHTTPi_error = 0xb;
  }
LAB_00010740:
  if (pNVar8 != NULL) {
    if (pNVar8->sslConn != NULL) {
      if (pNVar8->sslConn->server_name != NULL) {
        (*NHTTPi_free)(pNVar8->sslConn->server_name);
      }
      (*NHTTPi_free)(pNVar8->sslConn);
    }
    if (pNVar8->pURL != NULL) {
      (*NHTTPi_free)(pNVar8->pURL);
    }
    if (pNVar8->response != NULL) {
      (*NHTTPi_free)(pNVar8->response);
    }
    (*NHTTPi_free)(pNVar8);
  }
  return NULL;
}


void NHTTP_DestroyRequest(NHTTPReq *req)
{
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_request.c", 0x14e, "Failed assertion NHTTPi_isOpened");
  }
  if (req == (NHTTPReq *)0x0) {
    OSi_Panic("NHTTP_request.c", 0x14f, "Failed assertion req");
  }
  if (req->isStarted != 0) {
    OSi_Panic("NHTTP_request.c", 0x150, "Failed assertion !req->isStarted");
  }
  (*NHTTPi_free)(req->response);
  NHTTPi_destroyRequestObject(req);
  return;
}


int NHTTP_SendRequestAsync(NHTTPReq *req)
{
  int iVar1;
  
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_request.c", 0x18b, "Failed assertion NHTTPi_isOpened");
  }
  if (req == (NHTTPReq *)0x0) {
    OSi_Panic("NHTTP_request.c", 0x18c, "Failed assertion req");
  }
  if (req->isStarted == 0) {
    if ((req->method == NHTTP_REQMETHOD_POST) && (req->pListPost == (tagNHTTPi_DATALIST *)0x0)) {
      NHTTPi_error = 0xb;
      iVar1 = -1;
    }
    else {
      NHTTPi_lockReqList();
      iVar1 = NHTTPi_setReqQueue(req);
      if (iVar1 < 0) {
        NHTTPi_error = 1;
      } else {
        req->isStarted = 1;
        NHTTPi_kickCommThread();
      }
      NHTTPi_unlockReqList();
    }
  }
  else {
    NHTTPi_error = 0xb;
    iVar1 = -1;
  }
  return iVar1;
}


void NHTTPi_cancelAllRequests(void)
{
  NHTTPi_lockReqList();
  //if ((NHTTPi_reqCurrent != 0) && (*(int *)(*(int *)(NHTTPi_reqCurrent + 0xc) + 4) == 0)) {
  //  *(undefined4 *)(*(int *)(NHTTPi_reqCurrent + 0xc) + 4) = 1;
  //  NHTTPi_SocCancel(*(undefined4 *)(NHTTPi_reqCurrent + 0xc),
  //                   *(undefined4 *)(NHTTPi_reqCurrent + 0x10));
  //}
  //NHTTPi_allFreeReqQueue();
  NHTTPi_unlockReqList();
  return;
}


int NHTTPi_destroyRequestObject(NHTTPReq *req)
{
  deleteStrList(req->pListHeader);
  deleteStrList(req->pListPost);
  if (req->sslConn != NULL) {
    if (req->sslConn->server_name != NULL) {
      (*NHTTPi_free)(req->sslConn->server_name);
    }
    (*NHTTPi_free)(req->sslConn);
  }
  (*NHTTPi_free)(req->pURL);
  (*NHTTPi_free)(req);
  return 1;
}