#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

extern NHTTPError NHTTPi_error;
extern NHTTPAlloc NHTTPi_alloc;
extern NHTTPFree NHTTPi_free;
extern int NHTTPi_isOpened;

static BOOL addHdrList(tagNHTTPi_DATALIST **param_1, char *label, char *value, int param_4);
static BOOL checkTagPost(NHTTPReq *req, char *value, u32 length);
static char incAscii(char param_1);


static BOOL addHdrList(tagNHTTPi_DATALIST **param_1,char *label,char *value,int param_4)
{
  BOOL bVar1;
  int iVar2;
  tagNHTTPi_DATALIST *ptVar3;
  tagNHTTPi_DATALIST *unaff_r4;
  
  bVar1 = FALSE;
  if (*param_1 != (tagNHTTPi_DATALIST *)0x0) {
    unaff_r4 = *param_1;
    iVar2 = NHTTPi_compareToken(label,unaff_r4->label);
    if (iVar2 == 0) {
      bVar1 = TRUE;
    }
    else {
      for (unaff_r4 = unaff_r4->next; unaff_r4 != *param_1; unaff_r4 = unaff_r4->next) {
        iVar2 = NHTTPi_compareToken(label,unaff_r4->label);
        if (iVar2 == 0) {
          bVar1 = TRUE;
          break;
        }
      }
    }
  }
  if (bVar1) {
    unaff_r4->value = value;
  }
  else {
    ptVar3 = (tagNHTTPi_DATALIST *)(*NHTTPi_alloc)(param_4,4);
    if (ptVar3 == (tagNHTTPi_DATALIST *)0x0) {
      NHTTPi_error = 1;
      return 0;
    }
    ptVar3->label = label;
    ptVar3->value = value;
    ptVar3->length = 0;
    ptVar3->isBinary = 0;
    if (*param_1 == (tagNHTTPi_DATALIST *)0x0) {
      ptVar3->next = ptVar3;
      ptVar3->prev = ptVar3;
      *param_1 = ptVar3;
    }
    else {
      ptVar3->prev = (*param_1)->prev;
      ptVar3->next = *param_1;
      (*param_1)->prev->next = ptVar3;
      (*param_1)->prev = ptVar3;
    }
  }
  return 1;
}


static BOOL checkTagPost(NHTTPReq *req, char *value, u32 length)
{
  int iVar1;
  BOOL BVar2;
  int iVar3;
  uint uVar4;
  
  iVar1 = NHTTPi_memfind(value,length,req->tagPost + 2,0x12);
  if (iVar1 < 0) {
    BVar2 = 1;
  }
  else {
    for (iVar1 = 0x13; 1 < iVar1; iVar1 = iVar1 + -1) {
      uVar4 = (uint)req->tagPost[iVar1];
      while( TRUE ) {
        uVar4 = incAscii(uVar4 & 0xff);
        req->tagPost[iVar1] = (char)uVar4;
        //if (uVar4 == (int)(char)(&NHTTPi_strMultipartBound)[iVar1]) break; TODO
        iVar3 = NHTTPi_memfind(value,length,req->tagPost + 2,0x12);
        if (iVar3 < 0) {
          return 1;
        }
      }
    }
    BVar2 = 0;
  }
  return BVar2;
}


static char incAscii(char param_1)
{
  param_1 = param_1 + '\x01';
  if (param_1 == '{') {
    param_1 = '0';
  }
  else if (param_1 == '[') {
    param_1 = 'a';
  }
  else if (param_1 == ':') {
    param_1 = 'A';
  }
  return param_1;
}


BOOL NHTTP_AddPostDataAscii( NHTTPReq* req, char* label, char* value )
{
  unsigned long uVar1;
  int iVar2;
  BOOL BVar3;
  
  BVar3 = 0;
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_control.c", 0xca, "Failed assertion NHTTPi_isOpened");
  }
  if (req == NULL) {
    OSi_Panic("NHTTP_control.c", 0xcb, "Failed assertion req");
  }
  if (label == NULL) {
    OSi_Panic("NHTTP_control.c", 0xcc, "Failed assertion label");
  }
  if (value == NULL) {
    OSi_Panic("NHTTP_control.c", 0xcd, "Failed assertion value");
  }
  if (req->isStarted == 0) {
    uVar1 = NHTTPi_strlen(value);
    iVar2 = checkTagPost(req,value,uVar1);
    if ((iVar2 != 0) &&
       (BVar3 = addHdrList(&req->pListPost, label, value, 0x18), BVar3 != 0)) {
      req->pListPost->prev->length = uVar1;
    }
  } else {
    BVar3 = 0;
  }
  return BVar3;
}


BOOL NHTTP_AddPostDataBinary( NHTTPReq* req, char* label, char* value, u32 length )
{
  BOOL BVar1;
  BOOL BVar2;
  
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_control.c", 0xee, "Failed assertion NHTTPi_isOpened");
  }
  if (req == (NHTTPReq *)0x0) {
    OSi_Panic("NHTTP_control.c", 0xef, "Failed assertion req");
  }
  if (label == (char *)0x0) {
    OSi_Panic("NHTTP_control.c", 0xf0, "Failed assertion label");
  }

  if (req->isStarted == 0) {
    BVar1 = 0;
    BVar2 = checkTagPost(req,value,length);
    if ((BVar2 != 0) &&
       (BVar1 = addHdrList(&req->pListPost,label,value,0x18), BVar1 != 0)) {
      req->pListPost->prev->length = length;
      req->pListPost->prev->isBinary = 1;
    }
  }
  else {
    BVar1 = 0;
  }
  return BVar1;
}


BOOL NHTTP_AddHeaderField( NHTTPReq* req, char* label, char* value )
{
  BOOL BVar1;
  
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_control.c", 0xae, "Failed assertion NHTTPi_isOpened");
  }
  if (req == (NHTTPReq *)0x0) {
    OSi_Panic("NHTTP_control.c", 0xaf, "Failed assertion req");
  }
  if (label == (char *)0x0) {
    OSi_Panic("NHTTP_control.c", 0xb0, "Failed assertion label");
  }
  if (value == (char *)0x0) {
    OSi_Panic("NHTTP_control.c", 0xb1, "Failed assertion value");
  }
  if (req->isStarted == 0) {
    BVar1 = addHdrList(&req->pListHeader,label,value,0x18);
  }
  else {
    BVar1 = 0;
  }
  return BVar1;
}


BOOL NHTTP_SetCAChain( NHTTPReq* req, SOCCaInfo** cainfo, int cabuiltins )
{
  BOOL bVar1;
  
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_control.c", 0x10f, "Failed assertion NHTTPi_isOpened");
  }
  if (req == NULL) {
    OSi_Panic("NHTTP_control.c", 0x110, "Failed assertion req");
  }
  if (cainfo == NULL) {
    OSi_Panic("NHTTP_control.c", 0x111, "Failed assertion cainfo");
  }
  if (cabuiltins < 1) {
    OSi_Panic("NHTTP_control.c", 0x112, "Failed assertion cabuiltins > 0");
  }
  bVar1 = req->isStarted == 0;
  if (bVar1) {
    req->cainfo = cainfo;
    req->n_ca = cabuiltins;
  }
  return bVar1;
}


tagNHTTPi_DATALIST * NHTTPi_getHdrFromList(tagNHTTPi_DATALIST **param_1)
{
  tagNHTTPi_DATALIST *ptVar1;
  
  ptVar1 = *param_1;
  if (ptVar1 != (tagNHTTPi_DATALIST *)0x0) {
    if (ptVar1 == ptVar1->prev) {
      *param_1 = (tagNHTTPi_DATALIST *)0x0;
    }
    else {
      ptVar1->prev->next = ptVar1->next;
      ptVar1->next->prev = ptVar1->prev;
      *param_1 = ptVar1->next;
    }
  }
  return ptVar1;
}