#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"


static int getcharFromHdrRecvBuf(NHTTPRes *res, char *param_2, char *param_3);
static void setupGetcharFromHdrRecvBuf(NHTTPRes *res, int param_2, char *param_3, char *param_4);


static int getcharFromHdrRecvBuf(NHTTPRes *res, char *param_2, char *param_3)
{
  char cVar1;
  int iVar2;
  
  if (*(int *)param_2 == 0) {
    if (*(int *)param_3 < 0x400) {
      iVar2 = *(int *)param_3;
      *(int *)param_3 = *(int *)param_3 + 1;
      cVar1 = res->pHdrBufFirst[iVar2];
      goto LAB_00010100;
    }
    param_3[0] = '\0';
    param_3[1] = '\0';
    param_3[2] = '\0';
    param_3[3] = '\0';
    *(tagNHTTPi_HDRBUFLIST **)param_2 = res->pHdrBufBlock;
  }
  else if (*(int *)param_3 == 0x200) {
    param_3[0] = '\0';
    param_3[1] = '\0';
    param_3[2] = '\0';
    param_3[3] = '\0';
    *(undefined4 *)param_2 = **(undefined4 **)param_2;
  }
  iVar2 = *(int *)param_3;
  *(int *)param_3 = *(int *)param_3 + 1;
  cVar1 = *(char *)(*(int *)param_2 + iVar2 + 4);
LAB_00010100:
  return (int)cVar1;
}


static void setupGetcharFromHdrRecvBuf(NHTTPRes *res, int param_2, char *param_3, char *param_4)
{
  tagNHTTPi_HDRBUFLIST *ptVar1;
  int iVar2;
  
  if (param_2 < 0x400) {
    param_3[0] = '\0';
    param_3[1] = '\0';
    param_3[2] = '\0';
    param_3[3] = '\0';
    *(int *)param_4 = param_2;
  }
  else {
    ptVar1 = res->pHdrBufBlock;
    iVar2 = (int)(param_2 - 0x400U) >> 9;
    while (iVar2 != 0) {
      ptVar1 = ptVar1->next;
      iVar2 = iVar2 + -1;
    }
    *(tagNHTTPi_HDRBUFLIST **)param_3 = ptVar1;
    *(uint *)param_4 = param_2 - 0x400U & 0x1ff;
  }
  return;
}


int NHTTPi_compareTokenN_HdrRecvBuf(NHTTPRes *res,int param_2,int param_3,char *param_4,char param_5)
{
  BOOL bVar1;
  int iVar2;
  int iVar3;
  char auStack_20 [4];
  char auStack_1c [4];
  
  if (param_2 < param_3) {
    setupGetcharFromHdrRecvBuf(res,param_2,auStack_1c,auStack_20);
    iVar2 = getcharFromHdrRecvBuf(res,auStack_1c,auStack_20);
    while( TRUE ) {
      bVar1 = FALSE;
      if (('@' < *param_4) && (*param_4 < '[')) {
        bVar1 = TRUE;
      }
      if (bVar1) {
        iVar3 = *param_4 + 0x20;
      }
      else {
        iVar3 = (int)*param_4;
      }
      bVar1 = FALSE;
      if ((0x40 < iVar2) && (iVar2 < 0x5b)) {
        bVar1 = TRUE;
      }
      if (bVar1) {
        iVar2 = iVar2 + 0x20;
      }
      if (iVar2 != iVar3) goto LAB_00010360;
      if ((((*param_4 == '\0') || (*param_4 == ' ')) || (*param_4 == param_5)) ||
         (param_2 == param_3 + -1)) break;
      iVar2 = getcharFromHdrRecvBuf(res,auStack_1c,auStack_20);
      param_2 = param_2 + 1;
      param_4 = param_4 + 1;
    }
    iVar2 = 0;
  }
  else {
LAB_00010360:
    iVar2 = -1;
  }
  return iVar2;
}


int NHTTPi_findNextLineHdrRecvBuf(NHTTPRes *res, int param_2, int param_3, int *param_4)
{
  BOOL bVar1;
  int iVar2;
  char acStack_28 [4];
  char acStack_24 [4];
  int *piStack_20;
  
  bVar1 = FALSE;
  if (param_4 != (int *)0x0) {
    *param_4 = -1;
  }
  if (param_2 < param_3) {
    piStack_20 = param_4;
    setupGetcharFromHdrRecvBuf(res,param_2,acStack_24,acStack_28);
    for (; param_2 < param_3; param_2 = param_2 + 1) {
      iVar2 = getcharFromHdrRecvBuf(res,acStack_24,acStack_28);
      if (((iVar2 == 0x3a) && (param_4 != (int *)0x0)) && (*param_4 < 0)) {
        *param_4 = param_2;
      }
      if (bVar1) {
        if (iVar2 == 10) {
          if (param_2 == param_3 + -1) {
            return 0;
          }
          return param_2 + 1;
        }
        bVar1 = FALSE;
      }
      else if (iVar2 == 0xd) {
        bVar1 = TRUE;
      }
    }
  }
  return -1;
}


BOOL NHTTPi_isRecvBufFull(NHTTPRes *res,uint len)
{
    return res->nLenRecvBuf <= len;
}


int NHTTPi_loadFromHdrRecvBuf(NHTTPRes *res, char *param_2, int param_3, int param_4)
{
  int iVar1;
  uint uVar2;
  tagNHTTPi_HDRBUFLIST *ptVar3;
  
  if (res->nLenHeader < param_3 + param_4) {
    iVar1 = 0;
  }
  else {
    if (param_4 != 0) {
      if (param_3 < 0x400) {
        iVar1 = param_4;
        if (0x400 - param_3 < param_4) {
          iVar1 = 0x400 - param_3;
        }
        NHTTPi_memcpy(param_2,res->pHdrBufFirst + param_3,iVar1);
        param_3 = param_3 + iVar1;
        param_4 = param_4 - iVar1;
        param_2 = param_2 + iVar1;
      }
      if (param_4 != 0) {
        uVar2 = param_3 - 0x400;
        ptVar3 = res->pHdrBufBlock;
        iVar1 = (int)uVar2 >> 9;
        while (iVar1 != 0) {
          ptVar3 = ptVar3->next;
          iVar1 = iVar1 + -1;
        }
        for (; uVar2 = uVar2 & 0x1ff, param_4 != 0; param_4 = param_4 - iVar1) {
          iVar1 = param_4;
          if ((int)(0x200 - uVar2) < param_4) {
            iVar1 = 0x200 - uVar2;
          }
          NHTTPi_memcpy(param_2,ptVar3->block + uVar2,iVar1);
          ptVar3 = ptVar3->next;
          uVar2 = uVar2 + iVar1;
          param_2 = param_2 + iVar1;
        }
      }
    }
    iVar1 = 1;
  }
  return iVar1;
}


int NHTTPi_RecvBuf(NHTTPReq *req,int socket,int offset,int recvFlags)
{
  return NHTTPi_SocRecv(req,socket,req->response->pRecvBuf + offset,req->response->nLenRecvBuf - offset,
                 recvFlags);
  
}


int NHTTPi_RecvBufN(NHTTPReq *req,int socket,int offset,int len,int recvFlags)
{
  int iVar1;
  NHTTPRes *res;
  
  res = req->response;
  iVar1 = NHTTPi_isRecvBufFull(res,offset);
  if (iVar1 == 0) {
    if ((int)(res->nLenRecvBuf - offset) < len) {
      len = res->nLenRecvBuf - offset;
    }
    iVar1 = NHTTPi_SocRecv(req,socket,res->pRecvBuf + offset,len,recvFlags);
  } else {
    iVar1 = -0x3eb;
  }
  return iVar1;
}


int NHTTPi_skipSpaceHdrRecvBuf(NHTTPRes *param_1, int param_2, int param_3)
{
  int iVar1;
  char acStack_18 [4];
  char acStack_14 [4];
  
  if (param_2 < param_3) {
    setupGetcharFromHdrRecvBuf(param_1,param_2,acStack_14,acStack_18);
    for (; param_2 < param_3; param_2 = param_2 + 1) {
      iVar1 = getcharFromHdrRecvBuf(param_1,acStack_14,acStack_18);
      if (iVar1 != 0x20) {
        return param_2;
      }
    }
  }
  return -1;
}