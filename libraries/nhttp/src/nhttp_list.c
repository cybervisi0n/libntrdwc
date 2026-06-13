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


void NHTTPi_allFreeReqQueue(void)
{
  while (NHTTPi_reqQueue != (tagNHTTPi_REQLIST *)0x0) {
    NHTTPi_freeReqQueue(NHTTPi_reqQueue->id);
  }
  return;
}


tagNHTTPi_REQLIST * NHTTPi_findReqQueue(int id)
{
  tagNHTTPi_REQLIST *ptVar1;
  tagNHTTPi_REQLIST *ptVar2;
  
  ptVar1 = (tagNHTTPi_REQLIST *)0x0;
  if ((NHTTPi_reqQueue != (tagNHTTPi_REQLIST *)0x0) &&
     (ptVar1 = NHTTPi_reqQueue, NHTTPi_reqQueue->id != id)) {
    for (ptVar2 = NHTTPi_reqQueue->next;
        (ptVar1 = (tagNHTTPi_REQLIST *)0x0, ptVar2 != NHTTPi_reqQueue &&
        (ptVar1 = ptVar2, ptVar2->id != id)); ptVar2 = ptVar2->next) {
    }
  }
  return ptVar1;
}


int NHTTPi_freeReqQueue(int id)
{
  tagNHTTPi_REQLIST *ptVar1;
  int iVar2;
  NHTTPReqCallback p_Var3;
  _NHTTPRes *res;
  void *param;
  
  iVar2 = 0;
  ptVar1 = (tagNHTTPi_REQLIST *)NHTTPi_findReqQueue(id);
  if (ptVar1 != (tagNHTTPi_REQLIST *)0x0) {
    if (NHTTPi_reqQueue == NHTTPi_reqQueue->prev) {
      NHTTPi_reqQueue = NULL;
    } else {
      ptVar1->prev->next = ptVar1->next;
      ptVar1->next->prev = ptVar1->prev;
      if (NHTTPi_reqQueue == ptVar1) {
        NHTTPi_reqQueue = ptVar1->next;
      }
    }
    p_Var3 = ptVar1->req->pCallback;
    res = ptVar1->req->response;
    param = ptVar1->req->param;
    NHTTPi_destroyRequestObject(ptVar1->req);
    (*NHTTPi_free)(ptVar1);
    (*p_Var3)(NHTTP_ERROR_CANCELED,res,param);
    iVar2 = 1;
  }
  return iVar2;
}


tagNHTTPi_REQLIST * NHTTPi_getReqFromReqQueue(void)
{
  //return NHTTPi_getHdrFromList(NHTTPi_reqQueue);
  //This is a version of getHdrFromList using reqlist instead.
  tagNHTTPi_REQLIST ** param_1 = &NHTTPi_reqQueue;
  tagNHTTPi_REQLIST *ptVar1;
  
  ptVar1 = *param_1;
  if (ptVar1 != NULL) {
    if (ptVar1 == ptVar1->prev) {
      *param_1 = NULL;
    }
    else {
      ptVar1->prev->next = ptVar1->next;
      ptVar1->next->prev = ptVar1->prev;
      *param_1 = ptVar1->next;
    }
  }
  return ptVar1;
}


int NHTTPi_setReqQueue(NHTTPReq *param_1)
{
  tagNHTTPi_REQLIST *ptVar1;
  int iVar2;
  
  iVar2 = -1;
  ptVar1 = (tagNHTTPi_REQLIST *)(*NHTTPi_alloc)(0x14,4);
  if (ptVar1 != (tagNHTTPi_REQLIST *)0x0) {
    if (NHTTPi_reqQueue == (tagNHTTPi_REQLIST *)0x0) {
      ptVar1->prev = ptVar1;
      ptVar1->next = ptVar1;
      NHTTPi_reqQueue = ptVar1;
    }
    else {
      ptVar1->prev = NHTTPi_reqQueue->prev;
      ptVar1->next = NHTTPi_reqQueue;
      NHTTPi_reqQueue->prev->next = ptVar1;
      NHTTPi_reqQueue->prev = ptVar1;
    }
    iVar2 = NHTTPi_idRequest + 1;
    ptVar1->id = NHTTPi_idRequest;
    NHTTPi_idRequest = iVar2;
    ptVar1->req = param_1;
    ptVar1->socket = (void *)0xffffffff;
    iVar2 = ptVar1->id;
    if (NHTTPi_idRequest < 0) {
      NHTTPi_idRequest = 0;
    }
  }
  return iVar2;
}