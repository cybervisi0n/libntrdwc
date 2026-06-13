#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

extern int NHTTPi_isOpened;
extern tagNHTTPi_REQLIST * NHTTPi_reqQueue;
extern tagNHTTPi_REQLIST * NHTTPi_reqCurrent;
extern NHTTPFree NHTTPi_free;
extern NHTTPAlloc NHTTPi_alloc;
extern int NHTTPi_isThreadEnd;
extern NHTTPError NHTTPi_error;

static char g_commBuf[1024];


static void clearCloseQue(void);
static void freeSslConnection(CPSSslConnection *conn);
static void putSocketToCloseQue(int param_1,int *param_2);
static int saveBuf(NHTTPReq *param_1, int param_2,int *param_3,char *param_4,int param_5);
static int skipLineBuf(NHTTPReq *param_1,int param_2);


static void clearCloseQue(void)
{
  //for (; sdClosingQue_Wp != sdClosingQue_Rp; sdClosingQue_Rp = sdClosingQue_Rp + 1U & 7) {
  //  NHTTPi_SocCloseWait(0,sdClosingQue[sdClosingQue_Rp].s);
  //  freeSslConnection(sdClosingQue[sdClosingQue_Rp].con);
  //}
  return;
}


static void freeSslConnection(CPSSslConnection *conn)
{
  if (conn != NULL) {
    if (conn->server_name != NULL) {
      (*NHTTPi_free)(conn->server_name);
    }
    (*NHTTPi_free)(conn);
  }
  return;
}


static void putSocketToCloseQue(int param_1,int *param_2)
{
  //uint uVar1;
  //
  //uVar1 = sdClosingQue_Wp + 1U & 7;
  //if (uVar1 == sdClosingQue_Rp) {
  //  NHTTPi_SocCloseWait((NHTTPReq *)0x0,sdClosingQue[sdClosingQue_Rp].s);
  //  freeSslConnection(sdClosingQue[sdClosingQue_Rp].con);
  //  sdClosingQue_Rp = sdClosingQue_Rp + 1U & 7;
  //}
  //if (*param_2 != 0) {
  //  sdClosingQue[sdClosingQue_Wp].s = param_1;
  //  sdClosingQue[sdClosingQue_Wp].con = (_CPSSslConnection *)*param_2;
  //  *param_2 = 0;
  //  sdClosingQue_Wp = uVar1;
  //}
  return;
}


static int saveBuf(NHTTPReq *param_1, int param_2,int *param_3,char *param_4,int param_5)
{
  int iVar1;
  int iVar2;
  
  iVar2 = param_5;
  while( TRUE ) {
    if (iVar2 < 1) {
      return param_5;
    }
    if (param_1->doCancel != 0) break;
    iVar1 = iVar2;
    if (0x400 - *param_3 < iVar2) {
      iVar1 = 0x400 - *param_3;
    }
    NHTTPi_memcpy(g_commBuf + *param_3,param_4,iVar1);
    *param_3 = *param_3 + iVar1;
    param_4 = param_4 + iVar1;
    iVar2 = iVar2 - iVar1;
    if (*param_3 == 0x400) {
      iVar1 = NHTTPi_SocSend(param_1,param_2,g_commBuf,0x400,0);
      if (iVar1 < 1) {
        return iVar1;
      }
      *param_3 = *param_3 - iVar1;
    }
  }
  return -1;
}


static int skipLineBuf(NHTTPReq *param_1,int param_2)
{
  int iVar1;
  uint uVar2;
  int iVar3;
  char local_24 [4];
  
  iVar3 = 0;
  local_24[0] = '\0';
  local_24[1] = 0;
  uVar2 = 0;
  while (((local_24[uVar2 & 1] != '\r' || (iVar1 = iVar3, local_24[uVar2 - 1 & 1] != '\n')) &&
         (iVar1 = NHTTPi_SocRecv(param_1,param_2,local_24 + (uVar2 & 1),1,0), 0 < iVar1))) {
    iVar3 = iVar3 + iVar1;
    uVar2 = uVar2 + 1;
  }
  return iVar1;
}


BOOL NHTTP_GetProgress(u32 *received,u32 *contentlen)
{
  u32 uVar1;
  BOOL BVar2;
  
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_thread.c", 0x6f, "Failed assertion NHTTPi_isOpened");
  }
  if (received == NULL) {
    OSi_Panic("NHTTP_thread.c", 0x70, "Failed assertion received");
  }
  if (contentlen == NULL) {
    OSi_Panic("NHTTP_thread.c", 0x71, "Failed assertion contentlen");
  }
  *received = 0;
  *contentlen = 0;
  NHTTPi_lockReqList();
  if (NHTTPi_reqCurrent == NULL) {
    if (NHTTPi_reqQueue == NULL) {
      BVar2 = 0;
    } else {
      BVar2 = 1;
    }
  } else {
    if (NHTTPi_reqCurrent->req->nContentLength != 0) {
      *received = NHTTPi_reqCurrent->req->response->nLenBody;
      if (NHTTPi_reqCurrent->req->nContentLength == -1) {
        uVar1 = 0;
      } else {
        uVar1 = NHTTPi_reqCurrent->req->nContentLength;
      }
      *contentlen = uVar1;
    }
    BVar2 = 1;
  }
  NHTTPi_unlockReqList();
  #ifdef SDK_PORT
  //Prevents a divide by 0 error in the application
  return (*contentlen != 0) & BVar2;
  #else
  return BVar2;
  #endif
}



void NHTTPi_CommThreadProc(void * arg)
{
  BOOL bVar1;
  NHTTPReqMethod reqMethod;
  tagNHTTPi_DATALIST *ptVar3;
  int iVar4;
  tagNHTTPi_HDRBUFLIST *ptVar5;
  int iVar6;
  int iVar7;
  void *param;
  NHTTPReqCallback reqCallback;
  undefined4 in_r3;
  uint uVar9;
  NHTTPReq *req;
  int s;
  NHTTPRes *res;
  NHTTPError myError; //unaff_r10;
  int local_1ac;
  char local_1a8;
  char local_1a7;
  u8 local_1a6;
  uint local_1a4;
  int local_1a0;
  int local_19c;
  undefined4 local_198;
  int local_194;
  uint local_190;
  int local_18c;
  undefined4 local_188;
  int local_184;
  uint local_180;
  int local_17c;
  int local_178;
  undefined4 local_174;
  int local_170;
  undefined4 local_16c;
  int local_168;
  undefined4 local_164;
  int local_160;
  unsigned long local_15c;
  int local_158;
  undefined4 local_154;
  int local_150;
  undefined4 local_14c;
  int local_148;
  undefined4 local_144;
  int local_140;
  int local_13c;
  int local_138;
  undefined4 local_134;
  int local_130;
  undefined4 local_12c;
  int local_128;
  undefined4 local_124;
  int local_120;
  undefined4 local_11c;
  int local_118;
  undefined4 local_114;
  int local_110;
  uint local_10c;
  int local_108;
  undefined4 local_104;
  int local_100;
  undefined4 local_fc;
  int local_f8;
  undefined4 local_f4;
  int local_f0;
  undefined4 local_ec;
  int local_e8;
  undefined4 local_e4;
  int local_e0;
  undefined4 local_dc;
  int local_d8;
  int local_d4;
  int local_d0;
  undefined4 local_cc;
  int local_c8;
  int local_c4;
  int local_c0;
  undefined4 local_bc;
  int local_b8;
  int local_b4;
  int local_b0;
  undefined4 local_ac;
  int local_a8;
  undefined4 local_a4;
  int local_a0;
  undefined4 local_9c;
  int local_98;
  int local_94;
  int local_90;
  undefined4 local_8c;
  int local_88;
  undefined4 local_84;
  int local_80;
  undefined4 local_7c;
  void *local_78;
  undefined2 local_74;
  ushort local_72;
  tagNHTTPi_REQLIST *local_70;
  tagNHTTPi_HDRBUFLIST *local_6c;
  int local_68;
  uint local_64;
  uint local_60;
  int local_5c;
  int local_58;
  int local_54;
  u8 auStack_50 [4];
  int local_4c;
  int local_48;
  uint local_44;
  uint local_40;
  int local_3c;
  int local_38;
  CPSSslConnection *local_34;
  char local_30 [12];
  undefined4 uStack_24;
  
  s = -1;
  local_34 = NULL;
  local_3c = -1;
  local_58 = -1;
  local_5c = 0;
  local_60 = 0;
  local_64 = 0;
  local_68 = 0;
  //sdClosingQue_Wp = 0; //TODO
  //sdClosingQue_Rp = 0; //TODO
  uStack_24 = in_r3;
  do {
    while( TRUE ) {
      if (NHTTPi_isThreadEnd != 0) {
        #ifdef SDK_PORT
        if(s != 0)
        #else
        if (-1 < s) 
        #endif
        {
          NHTTPi_SocClose(req,s);
          putSocketToCloseQue(s,(int*)&local_34);
        }
        clearCloseQue();
        return;
      }
      NHTTPi_lockReqList();
      local_70 = NHTTPi_getReqFromReqQueue();
      if (local_70 == NULL) {
        local_4c = -1;
      } else {
        local_4c = local_70->id;
        req = local_70->req;
        NHTTPi_reqCurrent = local_70;
      }
      NHTTPi_unlockReqList();
      if (-1 < local_4c) break;
      NHTTPi_idleCommThread();
    }
    res = req->response;
    if (req->doCancel == 0) {
      local_38 = NHTTPi_resolveHostname(req);
      if (local_38 == 0) {
        NHTTPi_DBGOUT_STR_("NHTTP: cannot resolve hostname\n");
        myError = NHTTP_ERROR_DNS;
      } else {
        uVar9 = 0;
        bVar1 = FALSE;
        if ((local_38 == local_3c) && (req->port == local_58)) {
          bVar1 = TRUE;
        }
        if ((bVar1) && (req->isSSL == local_68)) {
          uVar9 = 1;
        }
        local_60 = local_60 & uVar9;
        local_58 = req->port;
        local_68 = req->isSSL;
        local_3c = local_38;
LAB_00010eb8:
        do {
          myError = NHTTP_ERROR_NONE;
          #ifdef SDK_PORT
          //TODO: Support KeepAlive
          local_60 = 0;
          #else
          if (-1 < s) {
            local_74 = 9;
            local_78 = (void *)s;
            //iVar7 = SOC_Poll(&local_78,1,0xcc8d,0);
            if (iVar7 < 1) {
              NHTTPi_DBGOUT_STR_("NHTTPi_CommThreadProc: SOC_Poll timeout or error. Stop Keep-Alive.\n");
              local_60 = 0;
            } else if ((local_72 & 0xe0) != 0) {
              NHTTPi_DBGOUT_STR_("NHTTPi_CommThreadProc: SOC_Poll detect remote disconnection. Stop Keep-Alive.\n");
              local_60 = 0;
            }
          }
          #endif
          if (local_60 == 0) {
            NHTTPi_DBGOUT_STR_("NHTTPi_CommThreadProc: !isKeepAlive...\n");
            #ifndef SDK_PORT
            //TODO: we should try to support this code
            if (-1 < s) 
            {
              iVar7 = NHTTPi_SocClose(req,s);
              if (iVar7 < 0) {
                myError = NHTTP_ERROR_NITROWIFI;
              }
              putSocketToCloseQue(s,(int*)&local_34);
              s = -1;
              if (myError != NHTTP_ERROR_NONE) goto LAB_00012abc;
            }
            #endif

            s = NHTTPi_SocOpen(req);
            #ifndef SDK_PORT
            if (s < 0) {
              myError = NHTTP_ERROR_SOCKET;
              goto LAB_00012abc;
            }
            #endif
            if (req->isSSL != 0) {
              local_34 = req->sslConn;
              req->sslConn = NULL;
            }
            NHTTPi_lockReqList();
            NHTTPi_reqCurrent->socket = (void *)s;
            NHTTPi_unlockReqList();
            if (req->doCancel != 0) goto LAB_00012abc;
            iVar7 = NHTTPi_SocConnect(req,s,local_38,req->port);
            local_60 = (uint)(-1 < iVar7);
          } else {
            NHTTPi_DBGOUT_STR_("NHTTPi_CommThreadProc: isKeepAlive...\n");
            if (req->isSSL != 0) {
              freeSslConnection(req->sslConn);
              req->sslConn = NULL;
            }
            NHTTPi_SetSSLseed();
            NHTTPi_lockReqList();
            NHTTPi_reqCurrent->socket = (void *)s;
            NHTTPi_unlockReqList();
          }
          if (req->doCancel != 0) goto LAB_00012abc;
          if (local_60 == 0) {
            myError = NHTTP_ERROR_CONNECT;
            goto LAB_00012abc;
          }
          local_60 = 0;
          local_40 = 0;
          local_48 = NHTTPi_strlen(req->pURL);
          myError = NHTTP_ERROR_NITROWIFI;
          reqMethod = req->method;
          if (reqMethod == NHTTP_REQMETHOD_GET) {
            local_7c = 4;
            local_80 = saveBuf(req,s,(int *)&local_40,"GET ",4);
            if (local_80 < 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
              goto LAB_00012abc;
            }
            if (local_80 == 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
              goto LAB_00010eb8;
            }
          } else if (reqMethod == NHTTP_REQMETHOD_POST) {
            local_84 = 5;
            local_88 = saveBuf(req,s,(int *)&local_40,"POST ",5);
            if (local_88 < 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
              goto LAB_00012abc;
            }
            if (local_88 == 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
              goto LAB_00010eb8;
            }
          } else if (reqMethod == NHTTP_REQMETHOD_HEAD) {
            local_8c = 5;
            local_90 = saveBuf(req,s,(int *)&local_40,"HEAD ",5);
            if (local_90 < 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
              goto LAB_00012abc;
            }
            if (local_90 == 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
              goto LAB_00010eb8;
            }
          }
          if (req->nLenHostDesc < local_48) {
            local_94 = local_48 - req->nLenHostDesc;
            if (local_94 != 0) {
              local_98 = saveBuf(req,s,(int *)&local_40,req->pURL + req->nLenHostDesc,local_94);
              if (local_98 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_98 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
            }
          } else {
            local_9c = 1;
            local_a0 = saveBuf(req,s,(int *)&local_40,"/",1);
            if (local_a0 < 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
              goto LAB_00012abc;
            }
            if (local_a0 == 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
              goto LAB_00010eb8;
            }
          }
          local_a4 = 0xb;
          local_a8 = saveBuf(req,s,(int *)&local_40," HTTP/1.1\r\n",0xb);
          if (local_a8 < 0) {
            NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
            goto LAB_00012abc;
          }
          if (local_a8 == 0) {
            NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
            goto LAB_00010eb8;
          }
          if (req->isSSL == 0) {
            local_44 = 7;
          } else {
            local_44 = 8;
          }
          local_ac = 6;
          local_b0 = saveBuf(req,s,(int *)&local_40,"Host: ",6);
          if (local_b0 < 0) {
            NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
            goto LAB_00012abc;
          }
          if (local_b0 == 0) {
            NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
            goto LAB_00010eb8;
          }
          local_b4 = req->nLenHost - local_44;
          if (local_b4 != 0) {
            local_b8 = saveBuf(req,s,(int *)&local_40,req->pURL + local_44,local_b4);
            if (local_b8 < 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
              goto LAB_00012abc;
            }
            if (local_b8 == 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
              goto LAB_00010eb8;
            }
          }
          local_bc = 2;
          local_c0 = saveBuf(req,s,(int *)&local_40,"\r\n",2);
          if (local_c0 < 0) {
            NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
            goto LAB_00012abc;
          }
          if (local_c0 == 0) {
            NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
            goto LAB_00010eb8;
          }
          ptVar3 = NHTTPi_getHdrFromList(&req->pListHeader);
          while (ptVar3 != (tagNHTTPi_DATALIST *)0x0) {
            local_c4 = NHTTPi_strlen(ptVar3->label);
            if (local_c4 != 0) {
              local_c8 = saveBuf(req,s,(int *)&local_40,ptVar3->label,local_c4);
              if (local_c8 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_c8 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
            }
            local_cc = 2;
            local_d0 = saveBuf(req,s,(int *)&local_40,": ",2);
            if (local_d0 < 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
              goto LAB_00012abc;
            }
            if (local_d0 == 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
              goto LAB_00010eb8;
            }
            local_d4 = NHTTPi_strlen(ptVar3->value);
            if (local_d4 != 0) {
              local_d8 = saveBuf(req,s,(int *)&local_40,ptVar3->value,local_d4);
              if (local_d8 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_d8 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
            }
            local_dc = 2;
            local_e0 = saveBuf(req,s,(int *)&local_40,"\r\n",2);
            if (local_e0 < 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
              goto LAB_00012abc;
            }
            if (local_e0 == 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
              goto LAB_00010eb8;
            }
            (*NHTTPi_free)(ptVar3);
            ptVar3 = NHTTPi_getHdrFromList(&req->pListHeader);
          }
          if (req->method == NHTTP_ERROR_ALLOC) {
            iVar7 = 0;
            local_5c = 0;
            for (ptVar3 = req->pListPost; ptVar3 != (tagNHTTPi_DATALIST *)0x0; ptVar3 = ptVar3->next
                ) {
              if (ptVar3->isBinary != 0) {
                local_5c = 1;
                break;
              }
              if (ptVar3 == req->pListPost->prev) break;
            }
            if (local_5c == 0) {
              for (ptVar3 = req->pListPost; ptVar3 != (tagNHTTPi_DATALIST *)0x0;
                  ptVar3 = ptVar3->next) {
                iVar6 = NHTTPi_getUrlEncodedSize(ptVar3->label);
                iVar4 = NHTTPi_getUrlEncodedSize(ptVar3->value);
                iVar7 = iVar7 + iVar6 + 1 + iVar4;
                if (ptVar3 == req->pListPost->prev) break;
                iVar7 = iVar7 + 1;
              }
              local_fc = 0x31;
              local_100 = saveBuf(req,s,(int *)&local_40,"Content-Type: application/x-www-form-urlencoded\r\n",0x31);
              if (local_100 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_100 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
            } else {
              for (ptVar3 = req->pListPost; ptVar3 != (tagNHTTPi_DATALIST *)0x0;
                  ptVar3 = ptVar3->next) {
                iVar6 = NHTTPi_strlen(ptVar3->label);
                iVar7 = iVar7 + iVar6 + 0x3f;
                if (ptVar3->isBinary != 0) {
                  iVar7 = iVar7 + 0x4b;
                }
                iVar7 = iVar7 + ptVar3->length + 4;
                if (ptVar3 == req->pListPost->prev) break;
              }
              iVar7 = iVar7 + 0x18;
              local_e4 = 0x2c;
              local_e8 = saveBuf(req,s,(int *)&local_40,"Content-Type: multipart/form-data; boundary=",0x2c);
              if (local_e8 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_e8 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
              local_ec = 0x12;
              local_f0 = saveBuf(req,s,(int *)&local_40,req->tagPost + 2,0x12);
              if (local_f0 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_f0 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
              local_f4 = 2;
              local_f8 = saveBuf(req,s,(int *)&local_40,"\r\n",2);
              if (local_f8 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_f8 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
            }
            local_104 = 0x10;
            local_108 = saveBuf(req,s,(int *)&local_40,"Content-Length: ",0x10);
            if (local_108 < 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
              goto LAB_00012abc;
            }
            if (local_108 == 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
              goto LAB_00010eb8;
            }
            local_10c = NHTTPi_intToStr(local_30,iVar7);
            local_44 = local_10c;
            if (local_10c != 0) {
              local_110 = saveBuf(req,s,(int *)&local_40,local_30,local_10c);
              if (local_110 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_110 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
            }
            local_114 = 2;
            local_118 = saveBuf(req,s,(int *)&local_40,"\r\n",2);
            if (local_118 < 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
              goto LAB_00012abc;
            }
            if (local_118 == 0) {
              NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
              goto LAB_00010eb8;
            }
          }
          local_11c = 2;
          local_120 = saveBuf(req,s,(int *)&local_40,"\r\n",2);
          if (local_120 < 0) {
            NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
            goto LAB_00012abc;
          }
          if (local_120 == 0) {
            NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
            goto LAB_00010eb8;
          }
          if (req->method == NHTTP_ERROR_ALLOC) {
            if (local_5c == 0) {
              for (ptVar3 = req->pListPost; ptVar3 != (tagNHTTPi_DATALIST *)0x0;
                  ptVar3 = ptVar3->next) {
                for (local_17c = 0; ptVar3->label[local_17c] != '\0'; local_17c = local_17c + 1) {
                  local_180 = NHTTPi_encodeUrlChar(local_30,(int)ptVar3->label[local_17c]);
                  local_44 = local_180;
                  if (local_180 != 0) {
                    local_184 = saveBuf(req,s,(int *)&local_40,local_30,local_180);
                    if (local_184 < 0) {
                      NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                      goto LAB_00012abc;
                    }
                    if (local_184 == 0) {
                      NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                      goto LAB_00010eb8;
                    }
                  }
                }
                local_188 = 1;
                local_18c = saveBuf(req,s,(int *)&local_40,"=",1);
                if (local_18c < 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                  goto LAB_00012abc;
                }
                if (local_18c == 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                  goto LAB_00010eb8;
                }
                for (local_17c = 0; ptVar3->value[local_17c] != '\0'; local_17c = local_17c + 1) {
                  local_190 = NHTTPi_encodeUrlChar(local_30,(int)ptVar3->value[local_17c]);
                  local_44 = local_190;
                  if (local_190 != 0) {
                    local_194 = saveBuf(req,s,(int *)&local_40,local_30,local_190);
                    if (local_194 < 0) {
                      NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                      goto LAB_00012abc;
                    }
                    if (local_194 == 0) {
                      NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                      goto LAB_00010eb8;
                    }
                  }
                }
                if (ptVar3 == req->pListPost->prev) break;
                local_198 = 1;
                local_19c = saveBuf(req,s,(int *)&local_40,"&",1);
                if (local_19c < 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                  goto LAB_00012abc;
                }
                if (local_19c == 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                  goto LAB_00010eb8;
                }
              }
            } else {
              for (ptVar3 = req->pListPost; ptVar3 != (tagNHTTPi_DATALIST *)0x0;
                  ptVar3 = ptVar3->next) {
                local_124 = 0x14;
                local_128 = saveBuf(req,s,(int *)&local_40,req->tagPost,0x14);
                if (local_128 < 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                  goto LAB_00012abc;
                }
                if (local_128 == 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                  goto LAB_00010eb8;
                }
                local_12c = 2;
                local_130 = saveBuf(req,s,(int *)&local_40,"\r\n",2);
                if (local_130 < 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                  goto LAB_00012abc;
                }
                if (local_130 == 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                  goto LAB_00010eb8;
                }
                local_134 = 0x26;
                local_138 = saveBuf(req,s,(int *)&local_40,"Content-Disposition: form-data; name=\""
                                    ,0x26);
                if (local_138 < 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                  goto LAB_00012abc;
                }
                if (local_138 == 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                  goto LAB_00010eb8;
                }
                local_13c = NHTTPi_strlen(ptVar3->label);
                if (local_13c != 0) {
                  local_140 = saveBuf(req,s,(int *)&local_40,ptVar3->label,local_13c);
                  if (local_140 < 0) {
                    NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                    goto LAB_00012abc;
                  }
                  if (local_140 == 0) {
                    NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                    goto LAB_00010eb8;
                  }
                }
                local_144 = 3;
                local_148 = saveBuf(req,s,(int *)&local_40,"\"\r\n",3);
                if (local_148 < 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                  goto LAB_00012abc;
                }
                if (local_148 == 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                  goto LAB_00010eb8;
                }
                if (ptVar3->isBinary != 0) {
                  local_14c = 0x4b;
                  local_150 = saveBuf(req,s,(int *)&local_40,
                                      "Content-Type: application/octet-stream\r\nContent-Transfer-En coding: binary\r\n"
                                      ,0x4b);
                  if (local_150 < 0) {
                    NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                    goto LAB_00012abc;
                  }
                  if (local_150 == 0) {
                    NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                    goto LAB_00010eb8;
                  }
                }
                local_154 = 2;
                local_158 = saveBuf(req,s,(int *)&local_40,"\r\n",2);
                if (local_158 < 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                  goto LAB_00012abc;
                }
                if (local_158 == 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                  goto LAB_00010eb8;
                }
                local_15c = ptVar3->length;
                if (local_15c != 0) {
                  local_160 = saveBuf(req,s,(int *)&local_40,ptVar3->value,local_15c);
                  if (local_160 < 0) {
                    NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                    goto LAB_00012abc;
                  }
                  if (local_160 == 0) {
                    NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                    goto LAB_00010eb8;
                  }
                }
                local_164 = 2;
                local_168 = saveBuf(req,s,(int *)&local_40,"\r\n",2);
                if (local_168 < 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                  goto LAB_00012abc;
                }
                if (local_168 == 0) {
                  NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                  goto LAB_00010eb8;
                }
                if (ptVar3 == req->pListPost->prev) break;
              }
              local_16c = 0x14;
              local_170 = saveBuf(req,s,(int *)&local_40,req->tagPost,0x14);
              if (local_170 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_170 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
              local_174 = 4;
              local_178 = saveBuf(req,s,(int *)&local_40,"--\r\n",4);
              if (local_178 < 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: send error\n");
                goto LAB_00012abc;
              }
              if (local_178 == 0) {
                NHTTPi_DBGOUT_STR_("NHTTP: connection reset. send retry...\n");
                goto LAB_00010eb8;
              }
            }
          }
          if ((int)local_40 < 1) break;
          local_1a0 = NHTTPi_SocSend(req,s,g_commBuf,local_40,0);
          if (local_1a0 < 0) goto LAB_00012abc;
        } while (local_1a0 == 0);
        NHTTPi_DBGOUT_STR_("NHTTP: end of sending\n");
        myError = NHTTP_ERROR_HTTPPARSE;
        res->nLenHeader = 0;
        local_30[0] = '\0';
        local_30[1] = 0;
        local_30[2] = 0;
        local_30[3] = 0;
        local_6c = res->pHdrBufBlock;
        local_40 = 0;
        do {
          if (req->doCancel != 0) goto LAB_00012abc;
          if ((int)local_40 < 0x400) {
            iVar7 = NHTTPi_SocRecv(req,s,res->pHdrBufFirst + local_40,1,0);
            local_30[local_40 & 3] = res->pHdrBufFirst[local_40];
          } else {
            local_1a4 = local_40 & 0x1ff;
            if (local_1a4 == 0) {
              if (local_6c == (tagNHTTPi_HDRBUFLIST *)0x0) {
                local_6c = (tagNHTTPi_HDRBUFLIST *)(*NHTTPi_alloc)(0x204,4);
                res->pHdrBufBlock = local_6c;
              } else {
                ptVar5 = (tagNHTTPi_HDRBUFLIST *)(*NHTTPi_alloc)(0x204,4);
                local_6c->next = ptVar5;
                local_6c = local_6c->next;
              }
              if (local_6c == (tagNHTTPi_HDRBUFLIST *)0x0) {
                myError = NHTTP_ERROR_ALLOC;
                goto LAB_00012abc;
              }
              local_6c->next = (tagNHTTPi_HDRBUFLIST *)0x0;
            }
            iVar7 = NHTTPi_SocRecv(req,s,local_6c->block + local_1a4,1,0);
            local_30[local_40 & 3] = local_6c->block[local_1a4];
          }
          if (iVar7 < 1) {
            myError = NHTTP_ERROR_NITROWIFI;
            goto LAB_00012abc;
          }
          local_40 = local_40 + iVar7;
        } while ((((local_30[local_40 - 4 & 3] != '\r') || (local_30[local_40 - 3 & 3] != '\n')) ||
                 (local_30[local_40 - 2 & 3] != '\r')) || (local_30[local_40 - 1 & 3] != '\n'));
        res->nLenHeader = local_40;
        if (res->nLenHeader == 0) {
          NHTTPi_DBGOUT_STR_("NHTTP: end of header not found\n");
        } else {
          NHTTPi_DBGOUT_STR_("NHTTP: header received\n");
          iVar7 = NHTTPi_loadFromHdrRecvBuf(res,g_commBuf,0,0xe);
          if (((iVar7 != 0) && (iVar7 = NHTTPi_strnicmp(g_commBuf,"HTTP/",5), iVar7 == 0)) &&
             ((g_commBuf[8] == ' ' &&
              ((local_54 = NHTTPi_strToInt(g_commBuf + 9,3), -1 < local_54 &&
               (iVar7 = NHTTPi_findNextLineHdrRecvBuf(res,0xc,res->nLenHeader,(int*)auStack_50), //not sure if the cast there is right
               -1 < iVar7)))))) {
            iVar7 = NHTTPi_getHeaderValue(res,"Content-Length",&local_44);
            if (iVar7 == 0) {
              myError = NHTTP_ERROR_NONE;
            } else if (iVar7 < 0x401) {
              if (iVar7 < 1) {
                req->nContentLength = -1;
              } else {
                iVar6 = NHTTPi_loadFromHdrRecvBuf(res,g_commBuf,local_44,iVar7);
                if ((iVar6 == 0) || (iVar7 = NHTTPi_strToInt(g_commBuf,iVar7), iVar7 < 0))
                goto LAB_00012abc;
                req->nContentLength = iVar7;
              }
              local_60 = NHTTPi_getHeaderValue(res,"Connection",&local_44);
              if (local_60 != 0) {
                if ((int)local_60 < 0) {
                  local_1a8 = g_commBuf[5];
                  local_1a7 = g_commBuf[7];
                  local_1a6 = 0;
                  iVar6 = NHTTPi_strToInt(&local_1a8,2);
                  local_60 = (uint)(10 < iVar6);
                } else if ((int)local_60 < 0x401) {
                  iVar6 = NHTTPi_compareTokenN_HdrRecvBuf(res,local_44,local_44 + local_60,"Keep-Alive",0);
                  if (iVar6 == 0) {
                    local_60 = 1;
                  } else {
                    local_60 = 0;
                  }
                } else {
                  local_60 = 0;
                }
                //NHTTPi_DBGOUT_ARG("NHTTP: isKeepAlive = %d\n",local_60);
                local_64 = NHTTPi_getHeaderValue(res,"Transfer-Encoding",&local_44);
                if (local_64 != 0) {
                  if ((int)local_64 < 0x401) {
                    if ((int)local_64 < 1) {
                      local_64 = 0;
                    } else {
                      iVar6 = NHTTPi_compareTokenN_HdrRecvBuf(res,local_44,local_44 + local_64,"chunked",0x3b);
                      local_64 = (uint)(iVar6 == 0);
                    }
                  } else {
                    local_64 = 0;
                  }
                  NHTTPi_DBGOUT_STR_("NHTTP: header parsed\n");
                  if (req->method != NHTTP_ERROR_TOOMANYREQ) {
                    if (iVar7 < 0) {
                      myError = NHTTP_ERROR_NITROWIFI;
                      if (local_64 != 0) {
                        do {
                          local_30[0] = '\0';
                          local_30[1] = 0;
                          for (local_40 = 0; (int)local_40 < 0x400; local_40 = local_40 + 1) {
                            iVar7 = NHTTPi_SocRecv(req,s,g_commBuf + local_40,1,0);
                            if (iVar7 < 0) {
                              //NHTTPi_DBGOUT_ARG("NHTTP: [%d] NHTTPi_SocRecv error = %d\n",local_4c,iVar7);
                              goto LAB_00012abc;
                            }
                            local_30[local_40 & 1] = g_commBuf[local_40];
                            if ((local_30[local_40 & 1] == ';') ||
                               ((local_30[local_40 & 1] == '\n' &&
                                (local_30[local_40 - 1 & 1] == '\r')))) {
                              if (local_30[local_40 & 1] == '\n') {
                                local_44 = local_40 - 1;
                              } else {
                                local_44 = local_40;
                                iVar7 = skipLineBuf(req,s);
                                if (iVar7 < 1) goto LAB_00012abc;
                              }
                              if ((local_44 == 0) ||
                                 (local_1ac = NHTTPi_strToHex(g_commBuf,local_44), local_1ac < 0))
                              goto LAB_00012abc;
                              break;
                            }
                          }
                          if (local_40 == 0x400) {
                            myError = NHTTP_ERROR_HTTPPARSE;
                            goto LAB_00012abc;
                          }
                          if (local_1ac < 1) {
                            skipLineBuf(req,s);
                            myError = NHTTP_ERROR_NONE;
                            goto LAB_00012abc;
                          }
                          while (0 < local_1ac) {
                            iVar7 = NHTTPi_RecvBufN(req,s,res->nLenBody,local_1ac,0);
                            if (iVar7 < 1) goto LAB_00012abc;
                            res->nLenBody = res->nLenBody + iVar7;
                            local_1ac = local_1ac - iVar7;
                            if ((local_1ac == 0) &&
                               ((iVar7 = NHTTPi_SocRecv(req,s,g_commBuf,1,0), iVar7 < 1 ||
                                (iVar7 = NHTTPi_SocRecv(req,s,g_commBuf,1,0), iVar7 < 1))))
                            goto LAB_00012abc;
                          }
                        } while( TRUE );
                      }
                      do {
                        do {
                          iVar7 = NHTTPi_isRecvBufFull(res,res->nLenBody);
                          if (iVar7 != 0) goto LAB_00012abc;
                          iVar7 = NHTTPi_RecvBuf(req,s,res->nLenBody,0);
                          if (iVar7 < 0) {
                            //NHTTPi_DBGOUT_ARG("NHTTP: [%d] NHTTPi_SocRecv error = %d\n",local_4c,iVar7);
                            goto LAB_00012abc;
                          }
                          if (iVar7 == 0) {
                            //NHTTPi_DBGOUT_ARG("NHTTP: [%d] Connection Closed.\n",local_4c);
                            myError = NHTTP_ERROR_NONE;
                            goto LAB_00012abc;
                          }
                          //NHTTPi_DBGOUT_ARG("NHTTP: [%d] %d received\n",local_4c,iVar7);
                          res->nLenBody = res->nLenBody + iVar7;
                          iVar7 = NHTTPi_isRecvBufFull(res,res->nLenBody);
                        } while (iVar7 == 0);
                        iVar7 = NHTTPi_SocRecv(req,s,g_commBuf,1,0);
                        if (iVar7 < 0) goto LAB_00012abc;
                      } while (iVar7 == 0);
                      myError = NHTTP_ERROR_BUFFULL;
                    } else {
                      while( TRUE ) {
                        if ((iVar7 < 1) ||
                           (iVar6 = NHTTPi_isRecvBufFull(res,res->nLenBody), iVar6 != 0))
                        goto LAB_00012760;
                        iVar6 = NHTTPi_RecvBufN(req,s,res->nLenBody,iVar7,0);
                        if (iVar6 < 0) {
                          //NHTTPi_DBGOUT_ARG("NHTTP: [%d] NHTTPi_SocRecv error = %d\n",local_4c,iVar6);
                          goto LAB_00012abc;
                        }
                        if (iVar6 == 0) break;
                        //NHTTPi_DBGOUT_ARG("NHTTP: [%d] %d received\n",local_4c,iVar6);
                        res->nLenBody = res->nLenBody + iVar6;
                        iVar7 = iVar7 - iVar6;
                      }
                      //NHTTPi_DBGOUT_ARG("NHTTP: [%d] Connection Closed.\n",local_4c);
LAB_00012760:
                      if (iVar7 == 0) {
                        myError = NHTTP_ERROR_NONE;
                      } else {
                        iVar7 = NHTTPi_isRecvBufFull(res,res->nLenBody);
                        if (iVar7 == 0) {
                          myError = NHTTP_ERROR_NITROWIFI;
                        } else {
                          myError = NHTTP_ERROR_BUFFULL;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
LAB_00012abc:
    NHTTPi_DBGOUT_STR_("NHTTP: finalizing communication\n");
    NHTTPi_lockReqList();
    (*NHTTPi_free)(NHTTPi_reqCurrent);
    NHTTPi_reqCurrent = NULL;
    NHTTPi_unlockReqList();
    if (req->doCancel != 0) {
      myError = NHTTP_ERROR_CANCELED;
      //NHTTPi_DBGOUT_ARG("NHTTP: [%d] request was canceled\n",local_4c);
    }
    #ifdef SDK_PORT
    if (((local_60 == 0 || (myError != NHTTP_ERROR_NONE))))
    #else
    if ((-1 < s) && ((local_60 == 0 || (myError != NHTTP_ERROR_NONE))))
    #endif
    {
      iVar7 = NHTTPi_SocClose(req,s);
      if (iVar7 < 0) {
        myError = NHTTP_ERROR_NITROWIFI;
      }
      putSocketToCloseQue(s, (int*)&local_34);
      s = -1;
      local_60 = 0;
    }
    if (myError == NHTTP_ERROR_NONE) {
      res->isSuccess = 1;
    } else {
      res->isSuccess = 0;
      NHTTPi_error = myError;
      //NHTTPi_DBGOUT_ARG("NHTTP: [%d] API error = %d\n", local_4c, myError);
    }
    param = req->param;
    reqCallback = req->pCallback;
    NHTTPi_destroyRequestObject(req);
    (*reqCallback)(myError, res, param);
    //NHTTPi_DBGOUT_ARG("NHTTP: [%d] complete\n" ,local_4c);
  } while( TRUE );

  return;
}