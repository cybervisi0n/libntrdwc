#include <nitro.h>
#include <nitroWiFi.h>
#include <string.h>
#include "auth/dwc_auth.h"
#include "auth/dwc_http.h"
#include "auth/dwc_netcheck.h"
#include "auth/dwc_svl.h"
#include "auth/util_alloc.h"
#include "auth/util_base64.h"
#include "decomp/decomp_defs.h"

#include "base/dwc_report.h"

typedef uchar undefined;

BOOL DWChttpignoreca = FALSE;

extern CPSCaInfo DWCca_nas_self;
extern CPSCaInfo DWCca_verisign_c3pcag3v2;
extern CPSCaInfo DWCac_verisign_c3pcag2v2;
extern CPSCaInfo DWCca_verisign_pca3ssg4;
extern CPSCaInfo DWCca_verisign_secureserver;
extern CPSCaInfo DWCca_cybertrust_globalsign;
extern CPSCaInfo DWCca_thawte_server;
extern CPSCaInfo DWCca_thawte_premiumserver;
extern CPSCaInfo DWCca_cybertrust_gteglroot;
extern CPSCaInfo DWCca_cybertrust_gteroot;
extern CPSCaInfo DWCca_cybertrust_baltimore;

CPSCaInfo *DWCcainfo[] = {
	&DWCca_verisign_c3pcag3v2,
	&DWCac_verisign_c3pcag2v2,
	&DWCca_verisign_pca3ssg4,
	&DWCca_verisign_secureserver,
	
	&DWCca_cybertrust_globalsign,
	&DWCca_cybertrust_gteglroot,
	&DWCca_cybertrust_gteroot,
	&DWCca_cybertrust_baltimore,
	
	&DWCca_thawte_server,
	&DWCca_thawte_premiumserver,
	
	&DWCca_nas_self
};

//undefined4 DWC_Http_Create(int param_1,undefined4 *param_2)
DWCHttpError DWC_Http_Create(DWCHttp *http, DWCHttpParam *param)
{
  int iVar1;
  undefined4 uVar2;
  DWCHttpAction myAction;
  undefined4 uVar3;
  undefined4 uVar4;
  //code *pcVar5;
  //void * pcVar5;
  void * ptr;

  //pcVar5 = (code *)param[3];
  //MI_CpuFill8(http,0,0x1c14);
  MI_CpuFill8(http, 0, sizeof(DWCHttp));
  //*(undefined4 *)(http + 0x1a30) = 0xffffffff;
  http->content_len = 0xffffffff;
  //*(undefined4 *)(http + 0x1a34) = 0xffffffff;
  http->receivedbody_len = 0xffffffff;
  //uVar2 = param[1];
  myAction = param->action;
  //uVar3 = param[2];
  uVar3 = param->len_recvbuf;
  //uVar4 = param[3];
  //param->alloc
  //PCPORT_TODO: These might not be right, the lining up of the offsets went weird with DWChttp
  //*(undefined4 *)(http + 0x1004) = *param;
  //*(undefined4 *)(http + 0x1008) = uVar2;
  //*(undefined4 *)(http + 0x100c) = uVar3;
  //*(undefined4 *)(http + 0x1010) = uVar4;
  http->param.url = param->url;
  http->param.action = param->action;
  http->param.len_recvbuf = param->len_recvbuf;
  http->param.alloc = param->alloc;
  //uVar2 = param[5];
  uVar2 = param->ignoreca;

  //uVar3 = param[6];
  uVar3 = param->timeout;
  //*(undefined4 *)(http + 0x1014) = param[4];
  http->param.free = param->free;
  //*(undefined4 *)(http + 0x1018) = uVar2;
  http->param.ignoreca = param->ignoreca;
  //*(undefined4 *)(http + 0x101c) = uVar3;
  http->param.timeout = param->timeout;
  //uVar2 = (*pcVar5)("ALLOC http->lowrecvbuf",0xb68);
  ptr = http->param.alloc((int)"ALLOC http->lowrecvbuf",0xb68);
  http->lowrecvbuf = ptr;
  if(http->lowrecvbuf == NULL) {
    http->error = DWCHTTP_E_MEMERR;
    uVar2 = 1;
  } else {
    //uVar2 = (*pcVar5)("ALLOC http->lowsendbuf",0x5ea);
    ptr = http->param.alloc((int)"ALLOC http->lowsendbuf",0x5ea);
    //*(undefined4 *)(http + 0x19d0) = uVar2;
    http->lowsendbuf = ptr;
    //if (*(int *)(http + 0x19d0) == 0) {
    if(http->lowsendbuf == NULL) {
      http->error = DWCHTTP_E_MEMERR;
      uVar2 = 1;
    } else {
      //iVar1 = DWCi_Http_AllocBuffer(http,http + 0x1a08,*(undefined4 *)(http + 0x100c));
      iVar1 = DWCi_Http_AllocBuffer(http,&http->rep,http->param.len_recvbuf);
      if (iVar1 == 0) {
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\t\tHttp : ??? FAILED TO ALLOC RECVBUF ???\n");
        http->error = DWCHTTP_E_MEMERR;
        uVar2 = 1;
      } else {
        iVar1 = DWCi_Http_SetHostinfo(http,param->url);
        if (iVar1 == 0) {
          DWC_Printf(DWC_REPORTFLAG_AUTH, "\t\tHttp : ??? FAILED TO SETHOSTINFO ???\n");
          http->error = DWCHTTP_E_MEMERR;
          uVar2 = 1;
        } else {
          uVar2 = DWCi_Http_WriteBasicHeader(http);
          http->error = uVar2;
          if(http->error == DWCHTTP_E_NOERR) {
            //*(undefined *)(http + 0x1000) = 0xff;
            http->initflag = 0xff;
          }
          uVar2 = http->error;
        }
      }
    }
  }
  return uVar2;
}

//undefined4 DWC_Http_FinishHeader(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)
DWCHttpError DWC_Http_FinishHeader(DWCHttp *http)
{
  int iVar1;
  undefined4 uVar2;
  char *pcVar3;
  size_t sVar4;
  undefined auStack_20 [8];
  undefined4 uStack_18;
  
  //uStack_18 = param_4;
  iVar1 = DWC_Http_Add_HeaderItem(http,"Connection","close");
  if (iVar1 == 0) {
    //pcVar3 = strstr(*(char **)(http + 0x19f8),"\x0d\x0a\x0d\x0a\x00");
    pcVar3 = strstr(http->req.buffer, "\x0d\x0a\x0d\x0a\x00");
    sVar4 = strlen(pcVar3 + 4);
    if (sVar4 != 0) {
      OS_SNPrintf(auStack_20,7,"%d",sVar4);
      iVar1 = DWC_Http_Add_HeaderItem(http,"Content-Length",auStack_20);
      if (iVar1 != 0) {
        return 1;
      }
    }
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

void DWC_Http_StartThread(DWCHttp *http, u32 prio)
{
  int iVar1;
  
  http->abort = 0;
  OS_InitMutex(&http->mutex);
  //OS_InitMutex(http + 0x1a18);
  OS_InitMutex(&http->content_len_mutex);
  //DWChttpignoreca = (int)(*(int *)(http + 0x1018) == 1);
  DWChttpignoreca = http->param.ignoreca;
  //if ((*(int *)(http + 0x1ba4) == 0) ||
  if((http->thread.state == OS_THREAD_STATE_WAITING) ||
  //   (iVar1 = OS_IsThreadTerminated(http + 0x1b38), iVar1 != 0)) {
       (iVar1 = OS_IsThreadTerminated(&http->thread), iVar1 != 0)) {
  //  OS_CreateThread(http + 0x1b38,DWCi_Http_Thread,http,http + 0x1000,0x1000,prio);
      OS_CreateThread(&http->thread, DWCi_Http_Thread, http, ((void*)http+0x1000),0x1000,prio);
  //  OS_WakeupThreadDirect(http + 0x1b38);
      OS_WakeupThreadDirect(&http->thread);
  }
  return;
}

void DWC_Http_Abort(DWCHttp *http)
{
  if (*(char *)(http + 0x1000) == -1) {
    DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : Locking mutex\n");
    OS_LockMutex(&http->mutex);
    http->abort = 1;
    OS_UnlockMutex(&http->mutex);
    DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : Mutex unlocked\n");
    //if (*(int *)(http + 0x1ba4) != 0) {
    //  OS_JoinThread(http->thread);
    //}
  } else {
    DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : This DWC_Http is already destroyed\n");
  }
  return;
}

BOOL DWCi_Http_Yield(DWCHttp * aHttp)
{
  BOOL bVar1;
  
  if (aHttp->ssl_enabled == 1) {
    //OS_GetLowEntropyData(aHttp + 0x19d4);
    OS_GetLowEntropyData(aHttp->lowentropydata);
    //CPS_SslAddRandomSeed(aHttp + 0x19d4,0x20);
    CPS_SslAddRandomSeed(aHttp->lowentropydata, 0x20);
  }
  OS_LockMutex(&aHttp->mutex);
  bVar1 = aHttp->abort != 1;
  if (bVar1) {
    OS_UnlockMutex(&aHttp->mutex);
    OS_Sleep(10);
  } else {
    OS_UnlockMutex(&aHttp->mutex);
  }
  return bVar1;
}

int DWC_Http_GetRecvProgress(DWCHttp *http)
{
  undefined4 uVar1;
  
  OS_LockMutex(&http->content_len_mutex);
  if ((*(int *)(http + 0x1a34) < 0) || (*(int *)(http + 0x1a30) < 0)) {
    OS_UnlockMutex(&http->content_len_mutex);
    uVar1 = 0xffffffff;
  } else {
    //uVar1 = _s32_div_f(*(undefined4 *)(http + 0x1a34),*(undefined4 *)(http + 0x1a30));
    OS_UnlockMutex(&http->content_len_mutex);
  }
  return uVar1;
}

void DWCi_Http_InitCpsSocket(DWCHttp * aHttp)
{
  MI_CpuClear8(&aHttp->soc, sizeof(CPSSoc));
  //*(undefined4 *)(param_1 + 0x1174) = 0xb68;
  aHttp->soc.rcvbuf.size = 0xb68;
  //*(undefined4 *)(param_1 + 0x1178) = *(undefined4 *)(param_1 + 0x19cc);
  aHttp->soc.rcvbuf.data = aHttp->lowrecvbuf;
  //*(undefined4 *)(param_1 + 0x1180) = 0x5ea;
  aHttp->soc.sndbuf.size = 0x5ea;
  //*(undefined4 *)(param_1 + 0x1184) = *(undefined4 *)(param_1 + 0x19d0);
  aHttp->soc.sndbuf.data = aHttp->lowsendbuf;
  CPS_SocRegister(&aHttp->soc);
  return;
}

CPSInAddr DWCi_Http_Resolve(DWCHttp * aHttp)
{
  CPSInAddr uVar1;
  //
  //DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : Resolving %s ...\n",*(undefined4 *)(aHttp + 0x1124));
  DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : Resolving %s ...\n", aHttp->hostname);
  //uVar1 = CPS_Resolve(*(undefined4 *)(aHttp + 0x1124));
  uVar1 = CPS_Resolve(aHttp->hostname);
  return uVar1;
}

undefined4 DWCi_Http_CheckHeaderReceived(DWCHttp * aHttp)
{
  char cVar1;
  char *pcVar2;
  undefined4 uVar3;
  size_t sVar4;
  char *pcVar5;
  int iVar6;
  u8 *puVar7;
  
  //puVar7 = (undefined4 *)(aHttp + 0x1a08);
  puVar7 = aHttp->rep.buffer;
  pcVar2 = strstr((char *)puVar7, "\x0d\x0a\x0d\x0a");
  if (pcVar2 == NULL) {
    uVar3 = 0;
  } else {
    pcVar2 = strstr((char *)puVar7,"\x0d\x0a\x0d\x0a");
    OS_LockMutex(&aHttp->content_len_mutex);
    *(int *)(aHttp + 0x1a34) = *(int *)(aHttp + 0x1a0c) - (int)(pcVar2 + 4);
    aHttp->receivedbody_len = (u64)(aHttp->rep.write_index) - ((u64)pcVar2) + 4;
    OS_UnlockMutex(&aHttp->content_len_mutex);
    pcVar2 = strstr((char *)puVar7, "Content-Length: ");
    if (pcVar2 == NULL) {
      uVar3 = 1;
    } else {
      sVar4 = strlen("Content-Length: ");
      pcVar5 = strstr(pcVar2 + sVar4,"\x0d\x0a");
      cVar1 = *pcVar5;
      *pcVar5 = '\0';
      OS_LockMutex(&aHttp->content_len_mutex);
      iVar6 = atoi(pcVar2 + sVar4);
      aHttp->content_len = iVar6;
      OS_UnlockMutex(&aHttp->content_len_mutex);
      *pcVar5 = cVar1;
      uVar3 = 1;
    }
  }
  return uVar3;
}

void DWCi_Http_Thread(void * arg)
{
  DWCHttp * http = (DWCHttp*)arg;
  longlong lVar1;
  int iVar2;
  size_t sVar3;
  uint uVar4;
  uint uVar5;
  BOOL bVar6;
  undefined8 uVar7;
  undefined8 uVar8;
  int local_50;
  uint local_48;
  uint local_3c;
  int local_38;
  u32 local_34;
  
  local_50 = 0;
  if (*(int *)(http + 0x101c) < 1) {
    local_48 = 60000;
  } else {
    local_48 = *(uint *)(http + 0x101c);
  }
  DWCi_Http_InitCpsSocket(http);
  iVar2 = DWCi_Http_Resolve(http);
  if (iVar2 == 0) {
    DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : ??? FAIL ???\n");
    http->error = DWCHTTP_E_DNSERR;
  } else {
    //*(int *)(http + 0x112c) = iVar2;
    http->hostip = iVar2;
    CPS_SocUse();
    if (http->ssl_enabled == 1) {
      MI_CpuClear8(&http->con, sizeof(CPSSslConnection));
      //http->con.auth_callback = DWCi_Http_CPSCallback;
      http->con.server_name = http->hostname;
      http->soc.con = &http->con;

      CPS_SetRootCa(DWCcainfo, sizeof(DWCcainfo)/sizeof(DWCcainfo[0]));
      CPS_SetSsl(1);
    }
    CPS_SocBind(0, http->port, iVar2);
    DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : TCP connect to %s:%d ...\n",http->hostname,http->port);
    iVar2 = CPS_TcpConnect();
    if (iVar2 == 0) {
      sVar3 = strlen(http->req.buffer);
      DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : Send HTTP request ... length=%d\n",sVar3);
      sVar3 = strlen(http->req.buffer);
      local_34 = CPS_SocWrite(http->req.buffer, sVar3);
      if ((int)local_34 < 1) {
        DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : ??? WRITEERROR ???\n");
        http->error = DWCHTTP_E_SENDERR;
      } else {
        CPS_SocFlush();
        iVar2 = DWCi_Http_Yield(http);
        if (iVar2 == 0) {
          http->error = DWCHTTP_E_ABORT;
        } else {
          DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : Receive HTTP reply ...\n");
          http->rep.write_index = http->rep.buffer;
          http->rep.buffer_tail = http->rep.buffer + http->rep.length;
          uVar7 = OS_GetTick();
          do {
            if (CPSMyIp == 0) {
              DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : ??? WCMERROR ???\n");
              http->error = DWCHTTP_E_SENDERR;
              goto LAB_00010d9c;
            }
            local_34 = CPS_SocGetLength();
            if ((int)local_34 < 0) {
              DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : ??? SOCGETCOUNT < 0 ???\n");
              goto LAB_00010d7c;
            }
            if (0 < (int)local_34) {
              uVar7 = OS_GetTick();
              u8 * tempBuf = NULL;
              /*iVar2*/tempBuf = CPS_SocRead(&local_34);
              if (tempBuf == 0) {
                DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\t Http : ??? SOCREAD NULL ???\n");
                goto LAB_00010d7c;
              }
              uVar5 = local_34;
              if((http->rep.buffer_tail - 1) - http->rep.write_index <= local_34) {
                uVar5 = (http->rep.buffer_tail - 1) - http->rep.write_index;
              }



              MI_CpuCopy8(tempBuf, http->rep.write_index, uVar5);
              http->rep.write_index = http->rep.write_index + uVar5;
              *(http->rep.write_index) = 0;
              if (local_50 == 1) {
                OS_LockMutex(&http->content_len_mutex);
                http->receivedbody_len = http->receivedbody_len + uVar5;
                OS_UnlockMutex(&http->content_len_mutex);
              } else {
                local_50 = DWCi_Http_CheckHeaderReceived(http);
              }
              DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : Receive progress %d/%d\n", http->receivedbody_len,
                         http->content_len);
              if (uVar5 < local_34) {
                CPS_SocConsume(local_34);
                DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : !!! RECVBUF FULL !!!\n");
                goto LAB_00010d7c;
              }
              CPS_SocConsume(uVar5);
            }
            local_38 = (int)((ulonglong)uVar7 >> 0x20);
            local_3c = (uint)uVar7;
            if ((-1 < http->content_len) &&
               (http->content_len <= http->receivedbody_len)) {
              DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : !!! CONTENT_LENGTH OK !!!\n");
LAB_00010d7c:
              CPS_TcpShutdown();
              CPS_TcpClose();
              CPS_SocRelease();
              CPS_SocUnRegister();
              http->error = DWCHTTP_E_FINISH;
              return;
            }
            uVar8 = OS_GetTick();
            lVar1 = (ulonglong)((uint)uVar8 - local_3c) * 0x40;
            //uVar8 = _ll_udiv((int)lVar1,
            //                 ((int)((ulonglong)uVar8 >> 0x20) -
            //                 (local_38 + (uint)((uint)uVar8 < local_3c))) * 0x40 +
            //                 (int)((ulonglong)lVar1 >> 0x20),0x82ea,0);
            uVar4 = (uint)((ulonglong)uVar8 >> 0x20);
            uVar5 = (int)local_48 >> 0x1f;
            bVar6 = uVar5 <= uVar4;
            if (uVar4 == uVar5) {
              bVar6 = local_48 <= (uint)uVar8;
            }
            #ifndef SDK_PORT
            //PCPORT_TODO
            if (bVar6 && (uVar4 != uVar5 || (uint)uVar8 != local_48)) {
              DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : ??? TIMEOUT ???\n");
              http->error = DWCHTTP_E_RECVTOUT;
              goto LAB_00010d9c;
            }
            #endif
            iVar2 = DWCi_Http_Yield(http);
          } while (iVar2 != 0);
          http->error = DWCHTTP_E_ABORT;
        }
      }
LAB_00010d9c:
      CPS_TcpShutdown();
      CPS_TcpClose();
      CPS_SocRelease();
      CPS_SocUnRegister();
    } else {
      DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : ??? FAIL ???\n");
      http->error = DWCHTTP_E_CONERR;
      CPS_SocRelease();
      CPS_SocUnRegister();
    }
  }
  return;
}

void DWC_Http_Destroy(DWCHttp *http)
{
  if (http != NULL) {
    //DWCi_Http_DestroyResult(http,http + 0x1a38,0x20,param_4,param_4);
    DWCi_Http_FreeBuffer(http, &http->rep);
    DWCi_Http_FreeBuffer(http, &http->req);
    if (http->lowrecvbuf != NULL) {
      http->param.free((int)"FREE http->lowrecvbuf", http->lowrecvbuf, 0);
      http->lowrecvbuf = NULL;
    }
    if (http->lowsendbuf != NULL) {
      http->param.free((int)"FREE http->lowsendbuf", http->lowsendbuf, 0);
      http->lowsendbuf = NULL;
    }

    MI_CpuFill8(http, 0, sizeof(DWCHttp));
  }
  return;
}

BOOL DWCi_Http_WriteBasicHeader(DWCHttp * aHttp)
{
  size_t hostnameLen;
  size_t reqFmtLen;
  size_t filepathLen;
  int iVar4;
  int iVar5;
  const char *reqFmt;
  
  //if (*(int *)(aHttp + 0x1008) == 0) {
  if(aHttp->param.action == DWCHTTP_POST){
    reqFmt = "POST /%s HTTP/1.0\r\nContent-type: application/x-www-form-urlencoded\r\nHost: %s\r\n\r\n";
  } else {
    reqFmt = "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n";
  }
  hostnameLen = strlen(aHttp->hostname);
  reqFmtLen = strlen(reqFmt);
  filepathLen = strlen(aHttp->filepath);
  iVar4 = DWCi_Http_AllocBuffer(aHttp, &aHttp->req, hostnameLen + reqFmtLen + filepathLen + 0x3fc);
  if (iVar4 == 1) {
    //iVar5 = OS_SNPrintf(*(undefined4 *)(aHttp + 0x19fc),*(undefined4 *)(aHttp + 0x1a04),__s,
    //                    *(undefined4 *)(aHttp + 0x1128),*(undefined4 *)(aHttp + 0x1124));
    iVar5 = OS_SNPrintf(aHttp->req.write_index, aHttp->req.length, reqFmt, aHttp->filepath, aHttp->hostname);
    aHttp->req.write_index = aHttp->req.write_index + iVar5;
  }
  return iVar4 != 1;
}

DWCHttpError DWC_Http_Add_HeaderItem(DWCHttp *http, char *label, char *data)
{
  char cVar1;
  size_t sVar2;
  size_t sVar3;
  size_t sVar4;
  undefined4 uVar5;
  char *pcVar6;
  int iVar7;
  int iVar8;
  
  sVar2 = strlen(data);
  sVar3 = strlen("%s: %s\r\n");
  sVar4 = strlen(label);
  iVar7 = sVar2 + (sVar3 - 4) + sVar4;
  //iVar8 = *(int *)(http + 0x1a00) - *(int *)(http + 0x19fc);
  iVar8 = (u64)http->req.buffer_tail - (u64)http->req.write_index;
  if ((iVar8 < iVar7 + 1) &&
     //(iVar8 = DWCi_Http_ReallocBuffer(http,(undefined4 *)(http + 0x19f8),(iVar7 - iVar8) + 1),
     (iVar8 = DWCi_Http_ReallocBuffer(http, &http->req, (iVar7-iVar8)+1),
     iVar8 == 0)) {
    uVar5 = 1;
  } else {
    //pcVar6 = strstr(*(char **)(http + 0x19f8),"\x0d\x0a\x0d\x0a\x00");
    pcVar6 = strstr(http->req.buffer,"\x0d\x0a\x0d\x0a\x00");
    pcVar6 = pcVar6 + 2;
    cVar1 = *pcVar6;
    sVar2 = strlen(pcVar6);
    memmove(pcVar6 + iVar7,pcVar6,sVar2 + 1);
    iVar8 = OS_SNPrintf(pcVar6,iVar7 + 1,"%s: %s\r\n",label,data);
    pcVar6[iVar8] = cVar1;
    //*(int *)(http + 0x19fc) = *(int *)(http + 0x19fc) + iVar7;
    http->req.write_index = http->req.write_index + iVar7;
    uVar5 = 0;
  }
  return uVar5;
}

//BOOL DWC_Http_Add_PostBase64Item(int param_1,char *param_2,undefined4 param_3,undefined4 param_4)
DWCHttpError DWC_Http_Add_PostBase64Item(DWCHttp *http, const char *label, const char *data, unsigned long data_len)
{
	int result_len, result_base64, buf_remain;
	DWCHttpBuffer *temp_buffer;
	char *format;

	// [nakata] よく使うアドレスをテンポラリに入れておく
	temp_buffer = &http->req;
	
	// [nakata] GETとPOSTで書き込む文字列を変える
	if(http->num_postitem == 0)
		format = DWC_HTTP_POSTITEM_FORMAT_HEAD;
	else
		format = DWC_HTTP_POSTITEM_FORMAT_MIDDLE;
	
	http->num_postitem++;
	
	// [nakata] 引数が文字列だけなのでSNPrintfの結果長を知ることができる
	result_base64 = DWC_Auth_Base64Encode(data, data_len, NULL, 0);
	result_len = (int)strlen(format)-2+(int)strlen(label)+result_base64;
	buf_remain = temp_buffer->buffer_tail-temp_buffer->write_index;

	if(result_len > buf_remain) {
		// [nakata] 書き込みバッファの容量が足りない分を追加
		if(DWCi_Http_ReallocBuffer(http, temp_buffer, result_len-buf_remain+1) == FALSE)
			return DWCHTTP_E_MEMERR;
		buf_remain = temp_buffer->buffer_tail-temp_buffer->write_index;
	}
	
	// [nakata] SNPrintf開始
	temp_buffer->write_index += OS_SNPrintf(temp_buffer->write_index, (unsigned long)buf_remain, format, label);
	
	// [nakata] 値が入るべき部分をバッファとしてbase64エンコードを行う
	buf_remain = temp_buffer->buffer_tail-temp_buffer->write_index;
	if(DWC_Auth_Base64Encode(data, data_len, temp_buffer->write_index, (unsigned long)buf_remain-1) < 0) {
		return DWCHTTP_E_MEMERR;
	}
	
	temp_buffer->write_index += result_base64;
	*(temp_buffer->write_index) = 0x00;
	
	return DWCHTTP_E_NOERR;
}

DWCHttpError DWC_Http_Add_Body(DWCHttp *http, const char *data)
{
  size_t sVar1;
  size_t sVar2;
  int iVar3;
  
  sVar1 = strlen(data);
  //iVar3 = *(int *)(http + 0x1a00) - *(int *)(http + 0x19fc);
  iVar3 = http->req.buffer_tail - http->req.write_index;
  if (iVar3 < (int)sVar1) {
    //iVar3 = DWCi_Http_ReallocBuffer(http,http + 0x19f8,(sVar1 - iVar3) + 1);
    DWCi_Http_ReallocBuffer(http, &http->req, (sVar1-iVar3)+1);
    if (iVar3 == 0) {
      return TRUE;
    }
    //iVar3 = *(int *)(http + 0x1a00) - *(int *)(http + 0x19fc);
    iVar3 = http->req.buffer_tail - http->req.write_index;
  }
  //sVar2 = OS_SNPrintf(*(undefined4 *)(http + 0x19fc),iVar3,&_13711,data);
  sVar2 = OS_SNPrintf(http->req.write_index, iVar3, "%s", data);
  if (sVar2 == sVar1) {
    //*(int *)(http + 0x19fc) = *(int *)(http + 0x19fc) + sVar2;
    http->req.write_index = http->req.write_index + sVar2;
  } else {
    DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : DWC_Http_Add_Body Error!\n");
  }
  return sVar2 != sVar1;
}

uint DWCi_Http_CPSCallback(uint param_1)
{
  if ((param_1 & 0x8000) != 0) {
    DWC_Printf(DWC_REPORTFLAG_AUTH,"\t\tHttp : SSL Certificate is out-of-date\n");
    param_1 = param_1 & 0xffff7fff;
  }
  if ((param_1 & 0x4000) != 0) {
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\t\tHttp : Server name does not match\n");
  }
  switch(param_1 & 0xff) {
  case 0:
    break;
  case 1:
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\t\tHttp : no root CA installed\n");
    break;
  case 2:
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\t\tHttp : bad signature\n");
    break;
  case 3:
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\t\tHttp : unknown signature algorithm\n");
    break;
  case 4:
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\t\tHttp : unknown public key alrorithm\n");
  }
  if (DWChttpignoreca == 1) {
    param_1 = 0;
  }
  return param_1;
}

undefined4 DWCi_Http_AllocBuffer(DWCHttp * aHttp, DWCHttpBuffer * aBuf, int aLen)
{
  undefined4 uVar1;
  int iVar2;
  void * ptr;
  
  if (aLen == 0) {
    uVar1 = 0;
  } else {
    //iVar2 = (**(code **)(aHttp + 0x1010))("ALLOC buf->buffer",param_3,param_3,param_4,param_4);
    ptr = aHttp->param.alloc((int)"ALLOC buf->buffer", aLen);
    //*param_2 = iVar2;
    aBuf->buffer = ptr;
    //if (*param_2 == 0) {
    if( aBuf->buffer == NULL ) {
      uVar1 = 0;
    } else {
      //param_2[1] = *param_2;
      //param_2[3] = param_3;
      //param_2[2] = *param_2 + param_2[3];
      aBuf->write_index = aBuf->buffer;
      aBuf->length = aLen;
      aBuf->buffer_tail = (void*)aBuf->buffer + aBuf->length;
      uVar1 = 1;
    }
  }
  return uVar1;
}

void DWCi_Http_FreeBuffer(DWCHttp* aHttp , DWCHttpBuffer * aBuf)
{
  if (aBuf->buffer != NULL) {
    aHttp->param.free((int)"FREE buf->buffer", aBuf->buffer, 0);
  }
  MI_CpuClear8(aBuf, sizeof(DWCHttpBuffer));
  return;
}

undefined4 DWCi_Http_ReallocBuffer(DWCHttp * aHttp,DWCHttpBuffer *aBuffer,int aLen)
{
  undefined4 uVar1;
  #ifdef SDK_PORT
  u64 iVar2;
  #else
  int iVar2;
  #endif
  void * ptr;
  //code *pcVar3;
  
  //pcVar3 = *(code **)(param_1 + 0x1014);
  if (aLen < 1) {
    uVar1 = 0;
  } else {
    //iVar2 = (**(code **)(param_1 + 0x1010))("ALLOC newptr",param_2[3] + param_3,param_3,param_4,param_4);
    ptr = aHttp->param.alloc((int)"ALLOC newptr", aBuffer->length + aLen);
    //if (iVar2 == 0) {
    if( ptr == NULL ) {
      uVar1 = 0;
    } else {
      //MI_CpuCopy8(*param_2,iVar2,param_2[3]);
      MI_CpuCopy8(aBuffer->buffer, ptr, aBuffer->length);
      //(*pcVar3)("FREE buf->buffer",*param_2,0);
      aHttp->param.free((int)"FREE buf->buffer", aBuffer->buffer, 0);
      if (ptr == NULL) {
        uVar1 = 0;
      } else {
        //param_2[1] = param_2[1] + (iVar2 - *param_2);
        aBuffer->write_index = aBuffer->write_index + ((u64)ptr - (u64)aBuffer->buffer);
        //param_2[3] = param_2[3] + param_3;
        aBuffer->length = aBuffer->length + aLen;
        //*param_2 = iVar2;
        aBuffer->buffer = ptr;
        //param_2[2] = iVar2 + param_2[3];
        aBuffer->buffer_tail = ptr + aBuffer->length;
        uVar1 = 1;
      }
    }
  }
  return uVar1;
}

undefined4 DWCi_Http_SetHostinfo(DWCHttp *aHttp, char * aUrl)
{
  size_t sVar1;
  undefined4 uVar2;
  size_t sVar3;
  char *pcVar4;
  int iVar5;
  char *__nptr;
  
  __nptr = (char *)0x0;
  sVar1 = strlen(aUrl);
  if (sVar1 < 0x100) {
    //strncpy((char *)(aHttp + 0x1024),aUrl,0x100);
    strncpy(aHttp->url, aUrl, 0x100);
    sVar1 = strlen(aUrl);
    //sVar3 = strlen((char *)(aHttp + 0x1024));
    sVar3 = strlen(aHttp->url);
    if (sVar1 == sVar3) {
      //pcVar4 = strstr((char *)(aHttp + 0x1024),_13771);
      pcVar4 = strstr(aHttp->url, "http://");
      if (pcVar4 == (char *)0x0) {
        //pcVar4 = strstr((char *)(aHttp + 0x1024),_13772);
        pcVar4 = strstr(aHttp->url, "https://");
        if (pcVar4 == (char *)0x0) {
          //No http:// or https:// in URL
          return 0;
        }
        //Found https:// in URL -> This is SSL
        //*(char **)(aHttp + 0x1124) = pcVar4 + 8;
        aHttp->hostname = pcVar4 + 8;
        //*(undefined4 *)(aHttp + 0x1130) = 1;
        aHttp->ssl_enabled = TRUE;
        //*(undefined2 *)(aHttp + 0x1134) = 0x1bb;
        aHttp->port = 443;
      } else {
        //Found http:// in URL -> This is not SSL
        //*(int *)(aHttp + 0x1124) = aHttp + 0x102b;
        aHttp->hostname = &aHttp->url[7];
        //*(undefined4 *)(aHttp + 0x1130) = 0;
        aHttp->ssl_enabled = FALSE;
        //*(undefined2 *)(aHttp + 0x1134) = 0x50;
        aHttp->port = 80;
      }
      //pcVar4 = strstr(*(char **)(aHttp + 0x1124),(char *)&_13773);
      //Search for alternate port number in URL
      pcVar4 = strstr(aHttp->hostname, ":");
      if (pcVar4 != (char *)0x0) {
        //Alternate Port Number found
        __nptr = pcVar4 + 1;
        *pcVar4 = '\0';
      }
      //pcVar4 = strstr(*(char **)(aHttp + 0x1124),(char *)&_13774);
      //Search for start of filepath
      pcVar4 = strstr(aHttp->hostname, "/");
      if (pcVar4 == (char *)0x0) {
        //No filepath found
        //*(undefined4 *)(aHttp + 0x1128) = 0;
        aHttp->filepath = NULL;
      } else {
        *pcVar4 = '\0';
        //*(char **)(aHttp + 0x1128) = pcVar4 + 1;
        aHttp->filepath = pcVar4 + 1;
      }
      if (__nptr != (char *)0x0) {
        //Get the port number from the string and set in DWCHttp structure
        iVar5 = atoi(__nptr);
        //*(short *)(aHttp + 0x1134) = (short)iVar5;
        aHttp->port = (unsigned short)iVar5;
      }
      uVar2 = 1;
    }
    else {
      uVar2 = 0;
    }
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}

int DWCi_Http_AddResult(DWCHttp * aHttp, /*prev int* */ DWCHttpParseResult *aResult, char *aLabel, char *aContent)
{
  size_t sVar1;
  undefined4 uVar2;
  //code *pcVar3;
  //code *pcVar4;

  u8 * labelBuf;
  u8 * valueBuf;

  
  //pcVar3 = *(code **)(aHttp + 0x1010); //aHttp->param.alloc
  //pcVar4 = *(code **)(aHttp + 0x1014); //aHttp->param.free
  //if (aResult[2] <= aResult[1]) {
  if (aResult->index <= aResult->len) {
    sVar1 = strlen(aLabel);
    //uVar2 = ((int)"ALLOC result->entry[i].label",sVar1 + 1);
    labelBuf = aHttp->param.alloc((int)"ALLOC result->entry[i].label", sVar1 + 1);
    //*(undefined4 *)(*aResult + aResult[2] * 8) = uVar2;
    aResult->entry[aResult->index].label = labelBuf;

    //if (*(int *)(*aResult + aResult[2] * 8) != 0) {
    //if ((aResult->entry + aResult->index * 8) != 0) {
    if(aResult->entry[aResult->index].label != NULL){
      sVar1 = strlen(aContent);
     // uVar2 = (*pcVar3)("ALLOC result->entry[i].value",sVar1 + 1);
     valueBuf = aHttp->param.alloc((int)"ALLOC result->entry[i].value", sVar1 + 1);
      //*(undefined4 *)(*aResult + aResult[2] * 8 + 4) = uVar2;
      aResult->entry[aResult->index].value = valueBuf;
      //if (*(int *)(*aResult + aResult[2] * 8 + 4) != 0) {
      if (aResult->entry[aResult->index].value != NULL) {
        //strcpy(*(char **)(*aResult + aResult[2] * 8), aLabel);
        strcpy(aResult->entry[aResult->index].label, aLabel);
        //strcpy(*(char **)(*aResult + aResult[2] * 8 + 4),aContent);
        strcpy(aResult->entry[aResult->index].value, aContent);
        //aResult[2] = aResult[2] + 1;
        aResult->index = aResult->index + 1;
        return 1;
      }
    }
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\t\tHttp : failed to add result\n");
    //if (*(int *)(*aResult + aResult[2] * 8) != 0) {
    if (aResult->entry[aResult->index].label != NULL) {
      aHttp->param.free((int)"FREE result->entry[i].label", aResult->entry[aResult->index].label, 0);
      //*(undefined4 *)(*aResult + aResult[2] * 8) = 0;
      aResult->entry[aResult->index].label = NULL;
    }
    if (aResult->entry[aResult->index].value != NULL) {
      aHttp->param.free((int)"FREE result->entry[i].value", aResult->entry[aResult->index].value, 0);
      //*(undefined4 *)(*aResult + aResult[2] * 8 + 4) = 0;
      aResult->entry[aResult->index].value = NULL;
    }
  }
  return 0;
}

BOOL DWC_Http_ParseResult(DWCHttp *http, BOOL noparsebody)
{
  char cVar1;
  char *pcVar2;
  char *pcVar3;
  undefined4 uVar4;
  size_t sVar5;
  char *pcVar6;
  int iVar7;
  char *pcVar8;
  char *pcVar9;
  char *pcVar10;
  char unaff_r10;
  int local_40 [4];


  
  //local_40[0] = http + 0x1a38;
  //labelValuePtr = http->labelvalue;
  //local_40[1] = 0x20;
  //local_40[2] = 0;
  //local_40[3] = param_4;
  DWCHttpParseResult myParseResult;
  myParseResult.entry = http->labelvalue;
  myParseResult.len = 0x20;
  myParseResult.index = 0;

  MI_CpuFill8(http->labelvalue, 0, sizeof(http->labelvalue));
  //pcVar2 = *(char **)(http + 0x1a08);
  pcVar2 = http->rep.buffer;
  //pcVar3 = strstr(pcVar2,(char *)&_13554);
  pcVar3 = strstr(pcVar2, "\x0d\x0a\x0d\x0a");
  if (pcVar3 == NULL) {
    uVar4 = 0;
  } else {
    pcVar3 = pcVar3 + 4;
    sVar5 = strlen(pcVar3);
    pcVar6 = pcVar3 + sVar5;
    pcVar2 = strstr(pcVar2, " ");
    if (pcVar2 == NULL) {
      uVar4 = 0;
    } else {
      cVar1 = pcVar2[4];
      pcVar2[4] = '\0';
      //iVar7 = DWCi_Http_AddResult(http,local_40,"httpresult",pcVar2 + 1);
      iVar7 = DWCi_Http_AddResult(http, &myParseResult, "httpresult", pcVar2 + 1);
      if (iVar7 == 1) {
        pcVar2[4] = cVar1;
        //if ((param_2 == 1) || (iVar7 = strncmp(pcVar2 + 1,(char *)&_13834,3), iVar7 != 0)) {
        iVar7 = strncmp(pcVar2 + 1, "\x0d\x0a", 3);
        if(noparsebody || iVar7 != 0) {
          uVar4 = 1;
        }
        if(0) {

        } else {
          pcVar2 = strstr(pcVar2 + 5, "\x0d\x0a");
          if (pcVar2 != (char *)0x0) {
            while( TRUE ) {
              pcVar10 = pcVar2 + 2;
              if (((*pcVar10 == '\r') || (pcVar2[3] == '\n')) ||
                 (pcVar9 = strstr(pcVar10, ": "), pcVar9 == (char *)0x0))
              goto LAB_00011dec;
              cVar1 = *pcVar9;
              *pcVar9 = '\0';
              pcVar2 = pcVar9 + 2;
              pcVar8 = strstr(pcVar2, "\x0d\x0a");
              if (pcVar8 == NULL) break;
              unaff_r10 = *pcVar8;
              *pcVar8 = '\0';
              iVar7 = DWCi_Http_AddResult(http, &myParseResult, pcVar10, pcVar2);
              if (iVar7 != 1) {
                *pcVar9 = cVar1;
                *pcVar8 = unaff_r10;
                return 0;
              }
              sVar5 = strlen(pcVar2);
              pcVar2 = pcVar2 + sVar5;
              *pcVar9 = cVar1;
              *pcVar8 = unaff_r10;
            }
            *pcVar9 = cVar1;
LAB_00011dec:
            while( TRUE ) {
              if ((pcVar6 <= pcVar3) ||
                 (pcVar2 = strstr(pcVar3, "="), pcVar2 == NULL)) {
                return 1;
              }
              cVar1 = *pcVar2;
              *pcVar2 = '\0';
              pcVar9 = pcVar2 + 1;
              pcVar10 = strstr(pcVar9, "&");
              if (pcVar10 == (char *)0x0) {
                pcVar10 = strstr(pcVar9,"\x0d\x0a");
              }
              if (pcVar10 != (char *)0x0) {
                unaff_r10 = *pcVar10;
                *pcVar10 = '\0';
              }
              iVar7 = DWCi_Http_AddResult(http, &myParseResult, pcVar3, pcVar9);
              if (iVar7 != 1) break;
              sVar5 = strlen(pcVar9);
              pcVar3 = pcVar9 + sVar5 + 1;
              *pcVar2 = cVar1;
              if (pcVar10 != (char *)0x0) {
                *pcVar10 = unaff_r10;
              }
            }
            *pcVar2 = cVar1;
            if (pcVar10 != (char *)0x0) {
              *pcVar10 = unaff_r10;
            }
            return 0;
          }
          uVar4 = 0;
        }
      } else {
        pcVar2[4] = cVar1;
        uVar4 = 0;
      }
    }
  }
  return uVar4;
}

char * DWC_Http_GetResult(DWCHttp *http, char *label)
{
  int iVar1;
  int idx;

  for(int idx = 0; idx < 0x20; idx++) {
    if(http->labelvalue[idx].label == NULL) {
      break;
    }
    
    if(strcmp(label, http->labelvalue[idx].label) == 0) {
      return http->labelvalue[idx].value;
    }
  }
  return NULL;
  ////while ((iVar2 < 0x20 && (*(int *)(http + iVar2 * 8 + 0x1a38) != 0))) {
  //while ((idx < 0x20 && http->labelvalue[idx] != 0))) {
  //  iVar1 = strcmp(label,*(char **)(http + idx * 8 + 0x1a38));
  //  if (iVar1 == 0) {
  //    //return *(undefined4 *)(http + idx * 8 + 0x1a3c);
  //    return NULL;
  //  }
  //  idx = idx + 1;
  //}
  //return 0;
}

//uint DWC_Http_GetBase64DecodedResult(undefined4 param_1,undefined4 param_2,int param_3,uint param_4)
int	DWC_Http_GetBase64DecodedResult(DWCHttp *http, char *label, char *buffer, unsigned long buffer_len)
{
  char *__s;
  uint uVar1;
  size_t sVar2;
  
  __s = (char *)DWC_Http_GetResult(http, label);
  if (__s == NULL) {
    uVar1 = 0;
  } else {
    sVar2 = strlen(__s);
    uVar1 = DWC_Auth_Base64Decode(__s,sVar2,buffer,buffer_len);
    if ((uVar1 != 0xffffffff) && (uVar1 < buffer_len)) {
      buffer[uVar1] = '\0';
    }
  }
  return uVar1;
}

//undefined4 DWC_Http_GetRawResult(undefined4 param_1,undefined4 param_2,char *param_3,int param_4)
BOOL	DWC_Http_GetRawResult(DWCHttp *http, char *label, char *buffer, int buffer_len)
{
  char *__s;
  undefined4 uVar1;
  size_t sVar2;
  
  //__s = (char *)DWC_Http_GetResult(http,label,buffer,buffer_len,buffer_len);
  if (__s == (char *)0x0) {
    uVar1 = 0;
  }
  else {
    sVar2 = strlen(__s);
    if ((int)sVar2 < buffer_len) {
      strcpy(buffer,__s);
      uVar1 = 1;
    }
    else {
      uVar1 = 0;
    }
  }
  return uVar1;
}

void DWCi_Http_DestroyResult(int param_1,int param_2,int param_3,undefined4 param_4)
{
  int iVar1;
  //code *pcVar2;
  
  //pcVar2 = *(code **)(param_1 + 0x1014);
  //DWC_Printf(0x1000000,_13863,param_3,param_4,param_4);
  //for (iVar1 = 0; iVar1 < param_3; iVar1 = iVar1 + 1) {
  //  if (*(int *)(param_2 + iVar1 * 8) != 0) {
  //    (*pcVar2)(_13864,*(undefined4 *)(param_2 + iVar1 * 8),0);
  //    *(undefined4 *)(param_2 + iVar1 * 8) = 0;
  //  }
  //  if (*(int *)(param_2 + iVar1 * 8 + 4) != 0) {
  //    (*pcVar2)(_13865,*(undefined4 *)(param_2 + iVar1 * 8 + 4),0);
  //    *(undefined4 *)(param_2 + iVar1 * 8 + 4) = 0;
  //  }
  //}
  return;
}