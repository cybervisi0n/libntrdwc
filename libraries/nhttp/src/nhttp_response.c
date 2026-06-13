#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

extern int NHTTPi_isOpened;
extern NHTTPFree NHTTPi_free;
extern NHTTPAlloc NHTTPi_alloc;
extern NHTTPError NHTTPi_error;


void NHTTP_DestroyResponse( NHTTPRes* res )
{
  tagNHTTPi_HDRBUFLIST *ptVar1;
  
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_response.c",0x33, "Failed assertion NHTTPi_isOpened");
  }
  if (res == NULL) {
    OSi_Panic("NHTTP_response.c",0x34, "Failed assertion res");
  }
  while (res->pHdrBufBlock != NULL) {
    ptVar1 = res->pHdrBufBlock->next;
    (*NHTTPi_free)(res->pHdrBufBlock);
    res->pHdrBufBlock = ptVar1;
  }
  if (res->allHeader != (char *)0x0) {
    (*NHTTPi_free)(res->allHeader);
  }
  if (res->foundHeader != (char *)0x0) {
    (*NHTTPi_free)(res->foundHeader);
  }
  (*NHTTPi_free)(res);
  return;
}


int NHTTP_GetBodyAll( NHTTPRes* res, char** value )
{
  int iVar1;
  
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_response.c",0xf9, "Failed assertion NHTTPi_isOpened");
  }
  if (res == (NHTTPRes *)0x0) {
    OSi_Panic("NHTTP_response.c",0xfa, "Failed assertion res");
  }
  if (value == (char **)0x0) {
    OSi_Panic("NHTTP_response.c",0xfb, "Failed assertion value");
  }
  if ((res->isSuccess == 0) || (res->nLenBody == 0)) {
    iVar1 = -1;
  } else {
    *value = res->pRecvBuf;
    iVar1 = res->nLenBody;
  }
  return iVar1;
}


int NHTTP_GetHeaderAll(NHTTPRes *res,char **value)
{
  int iVar1;
  char *pcVar2;
  
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_response.c",0xd0, "Failed assertion NHTTPi_isOpened");
  }
  if (res == (NHTTPRes *)0x0) {
    OSi_Panic("NHTTP_response.c",0xd1, "Failed assertion res");
  }
  if (value == (char **)0x0) {
    OSi_Panic("NHTTP_response.c",0xd2, "Failed assertion value");
  }
  if ((res->isSuccess == 0) || (res->nLenHeader == 0)) {
    iVar1 = -1;
  }
  else {
    if (res->nLenHeader < 0x400) {
      *value = res->pHdrBufFirst;
    }
    else {
      if (res->allHeader == (char *)0x0) {
        pcVar2 = (char *)(*NHTTPi_alloc)(res->nLenHeader,4);
        res->allHeader = pcVar2;
        if (res->allHeader == (char *)0x0) {
          NHTTPi_error = 1;
          return -1;
        }
        //NHTTPi_loadFromHdrRecvBuf(res,res->allHeader,0,res->nLenHeader);
      }
      *value = res->allHeader;
    }
    iVar1 = res->nLenHeader;
  }
  return iVar1;
}


int NHTTP_GetHeaderField( NHTTPRes* res, char* label, char** value )
{
  int iVar1;
  char *pcVar2;
  undefined4 in_r3;
  undefined4 local_18;
  
  local_18 = in_r3;
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_response.c",0x9f,"Failed assertion NHTTPi_isOpened");
  }
  if (res == (NHTTPRes *)0x0) {
    OSi_Panic("NHTTP_response.c",0xa0,"Failed assertion res");
  }
  if (label == NULL) {
    OSi_Panic("NHTTP_response.c",0xa1,"Failed assertion label");
  }
  if (value == NULL) {
    OSi_Panic("NHTTP_response.c",0xa2,"Failed assertion value");
  }
  if (res->isSuccess == 0) {
    iVar1 = -1;
  } else {
    if (res->foundHeader != NULL) {
      (*NHTTPi_free)(res->foundHeader);
      res->foundHeader = NULL;
    }
    iVar1 = NHTTPi_getHeaderValue(res,label,&local_18);
    if (iVar1 < 0) {
      iVar1 = NHTTPi_strcmp("HTTPSTATUSCODE",label);
      if (iVar1 == 0) {
        pcVar2 = (char *)(*NHTTPi_alloc)(4,4);
        res->foundHeader = pcVar2;
        res->foundHeader[3] = '\0';
        NHTTPi_loadFromHdrRecvBuf(res,res->foundHeader,9,3);
        *value = res->foundHeader;
        iVar1 = 3;
      } else {
        iVar1 = -1;
      }
    } else {
      pcVar2 = (char *)(*NHTTPi_alloc)(iVar1 + 1,4);
      res->foundHeader = pcVar2;
      res->foundHeader[iVar1] = '\0';
      NHTTPi_loadFromHdrRecvBuf(res,res->foundHeader,local_18,iVar1);
      *value = res->foundHeader;
    }
  }
  return iVar1;
}


int NHTTPi_getHeaderValue(NHTTPRes *res, char *label, int *param_3)
{
  int iVar1;
  int iVar2;
  int local_34;
  
  iVar1 = NHTTPi_findNextLineHdrRecvBuf(res,0xc,res->nLenHeader,&local_34);
  do {
    iVar2 = iVar1;
    if (iVar2 < 1) {
      return -1;
    }
    iVar1 = NHTTPi_findNextLineHdrRecvBuf(res,iVar2,res->nLenHeader,&local_34);
  } while ((local_34 < 1) ||
          (iVar2 = NHTTPi_compareTokenN_HdrRecvBuf(res,iVar2,local_34,label,0), iVar2 != 0));
  if (local_34 + 1 < res->nLenHeader) {
    iVar1 = NHTTPi_findNextLineHdrRecvBuf(res,local_34 + 1,res->nLenHeader,0);
    if (iVar1 < 1) {
      iVar1 = res->nLenHeader;
    }
    else {
      if (iVar1 < 2) {
        return -1;
      }
      iVar1 = iVar1 + -2;
    }
    iVar2 = NHTTPi_skipSpaceHdrRecvBuf(res,local_34 + 1,iVar1);
    if (iVar2 < 0) {
      iVar2 = iVar1;
    }
    *param_3 = iVar2;
    iVar1 = iVar1 - iVar2;
  }
  else {
    iVar1 = 0;
  }
  return iVar1;
}