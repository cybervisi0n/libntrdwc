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

#ifdef SDK_BUILD_LINUX
#include <errno.h>
#endif

typedef unsigned char   undefined;

struct CPSPrivateKey {
    int modulus_len;
    uchar *modulus;
    int prime1_len;
    uchar *prime1;
    int prime2_len;
    uchar *prime2;
    int exponent1_len;
    uchar *exponent1;
    int exponent2_len;
    uchar *exponent2;
    int coefficient_len;
    uchar *coefficient;
};

union anon_union_72_2_d93c2505_for_common1 {
    uchar sessionID[32];
    uchar key_block[72];
};

typedef struct _CPSSslConnection CPSSslConnection;

static u8 DWCallow302;
static DWCHttp * DWChttp;
static u8 DWCishotspot;
static DWCNetcheck * DWCnetcheck;
static DWCHttpParam DWCnetcheckhttpparam;
static DWCAuthParam DWCnetcheckauthparam;

static char * DWCnetcheckblurl = "http://conntest.nintendowifi.net/";
extern DWCHttpParam DWCauthhttpparam;

static u32 DWCconntestip;

static DWCSimpleNetcheck * DWCsimplecheckwork;


DWCNetcheckError DWC_Netcheck_Create(DWCNetcheckParam *param)
{
  DWCNetcheckError status;
  DWCNetcheckParam * myParam;
  DWCAuthAlloc myAlloc;
  
  myAlloc = param->alloc;
  if (DWCnetcheck == (DWCNetcheck *)0x0) {
    //DWCnetcheck = (DWCNetcheck *)(*pcVar6)(_13521,0x11f4,param_3,param_4,param_4);
    DWCnetcheck = (DWCNetcheck *)myAlloc(ALLOC_DWC_NETCHECK, sizeof(DWCNetcheck));
    if (DWCnetcheck == (DWCNetcheck *)0x0) {
      status = DWCNETCHECK_E_MEMERR;
    } else {
      MI_CpuClear8(DWCnetcheck,sizeof(DWCNetcheck));
      DWCnetcheck->returncode = -99999;

      myParam = &DWCnetcheck->param;
      myParam->alloc = myAlloc;
      myParam->free = param->free;
      myParam->bmworkarea = param->bmworkarea;
      if (DWChttp == (DWCHttp *)0x0) {
        DWChttp = (DWCHttp *)myAlloc(ALLOC_DWC_HTTP, sizeof(DWCHttp));
        if (DWChttp == (DWCHttp *)0x0) {
          status = 4;
        } else {
          DWCishotspot = 0;
          OS_InitMutex(&DWCnetcheck->mutex);
          DWCi_Netcheck_StartThread();
          status = DWCNETCHECK_E_NOERR;
        }
      } else {
        status = DWCNETCHECK_E_MEMERR;
      }
    }
  } else {
    status = DWCNETCHECK_E_MEMERR;
  }
  return status;
}

void DWC_Netcheck_SetAllow302(int param_1)
{
  DWCallow302 = param_1;
  return;
}

void DWC_Netcheck_Destroy(void)
{
  void * p_Var1;
  
  p_Var1 = (DWCnetcheck->param).free;
  if (DWChttp != NULL) {
    DWC_Http_Destroy(DWChttp);
    DWCnetcheck->param.free((unsigned long)"DWChttp", DWChttp, 0);
    DWChttp = NULL;
  }
  DWC_Auth_Destroy();
  if (DWCnetcheck != NULL) {
    if (DWCnetcheck->body_302 != NULL) {
      DWCnetcheck->param.free((unsigned long)"DWCnetcheck->body_302", DWCnetcheck->body_302, 0);
      DWCnetcheck->body_302 = NULL;
    }
    if (DWCnetcheck->body_wayport != NULL) {
      DWCnetcheck->param.free((unsigned long)"DWCnetcheck->body_wayport", DWCnetcheck->body_wayport, 0);
      DWCnetcheck->body_wayport = NULL;
    }
    DWCnetcheck->param.free((unsigned long)"DWCnetcheck", DWCnetcheck, 0);
    DWCnetcheck = NULL;
  }
  return;
}

void DWC_Netcheck_Abort(void)
{
  if(DWCnetcheck != NULL) {
    DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Aborting\n");
    if(DWChttp != NULL) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Http abort\n");
      DWC_Http_Abort(DWChttp);
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Http abort finished\n");
    }
    DWC_Printf(DWC_REPORTFLAG_AUTH,"Netcheck : Auth abort\n");
    DWC_Auth_Abort();
    DWC_Printf(DWC_REPORTFLAG_AUTH,"Netcheck : Auth abort finished\n");
    if ((DWCnetcheck->thread).id != 0) {
      OS_JoinThread(&DWCnetcheck->thread);
    }
    DWCnetcheck->returncode = -7;
    DWC_Printf(DWC_REPORTFLAG_AUTH,"Netcheck : Aborted\n");
  }
  return;
}

DWCNetcheckError DWC_Netcheck_GetError(void)
{
  DWCNetcheckError err;
  
  OS_LockMutex(&DWCnetcheck->mutex);
  err = DWCnetcheck->error;
  OS_UnlockMutex(&DWCnetcheck->mutex);
  return err;
}

int DWC_Netcheck_GetReturnCode(void)
{
  return DWCnetcheck->returncode;
}

void DWC_Netcheck_SetCustomBlacklist(const char *url)
{
  DWCnetcheckblurl = url;
}

void DWCi_Netcheck_StartThread(void)
{
  int status;
  
  if (((DWCnetcheck->thread).id == 0) ||
     (status = OS_IsThreadTerminated(&DWCnetcheck->thread), status != 0)) {
    OS_CreateThread(&DWCnetcheck->thread,DWCi_Netcheck_Thread,DWCnetcheck,&DWCnetcheck->error,0x1000
                    ,0x10);
    OS_WakeupThreadDirect(&DWCnetcheck->thread);
  }
}

void DWCi_Netcheck_Thread(void * arg)
{

  DWCWiFiInfo	DWC_netcheck_wifiinfo;
  DWCAuthResult authresult;

  int iVar1;
  size_t sVar2;
  char *pcVar3;
  int iVar4;
  //_enum _Var5;
  int _Var5;
  int iVar6;
  //_func_void_ptr_unsigned_long_long *p_Var7;
  //_func_void_unsigned_long_void_ptr_long *p_Var8;
  void *p_Var7;
  void *p_Var8;
  char *url;
  void *ptr_00;
  char *__nptr;
  int local_2a8;
  char acStack_29c [4];
  char *local_298;
  char *local_294;
  char * httpResult;
  int httpStatusCode;

  int local_28c;
  int local_288;
  char calInfoBuf[148];
  int local_1e4 [113];

  DWCHttpError httpStatus;
  
  //p_Var7 = DWCnetcheck->param.alloc;
  //p_Var8 = (DWCnetcheck->param).free;
  local_294 = (char *)0x0;
  local_298 = (char *)0x0;
  url = (char *)0x0;
  ptr_00 = (void *)0x0;
  __nptr = (char *)0x0;
  do {
    DWCnetcheckhttpparam.url                = DWCnetcheckblurl;
    DWCnetcheckhttpparam.action             = DWCHTTP_GET;
    DWCnetcheckhttpparam.len_recvbuf        = 0x1000;
    DWCnetcheckhttpparam.timeout            = 20000;
    DWCnetcheckhttpparam.alloc              = DWCnetcheck->param.alloc;
    DWCnetcheckhttpparam.free               = DWCnetcheck->param.free;

    DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Getting blacklist site\n");

    DWCnetcheck->returncode = -2;
    httpStatus = DWC_Http_Create(DWChttp,&DWCnetcheckhttpparam);
    if (httpStatus != DWCHTTP_E_NOERR) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(DWCNETCHECK_E_HTTPINITERR);
LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs:
      if (url != (char *)0x0) {
        DWCnetcheck->param.free((int)"FREE url", url, 0);
      }
      if (ptr_00 != (void *)0x0) {
        DWCnetcheck->param.free((int)"FREE data", ptr_00, 0);
      }
      if (__nptr != (char *)0x0) {
        DWCnetcheck->param.free((int)"FREE wait", __nptr, 0);
      }
      return;
    }

    iVar1 = DWC_Http_FinishHeader(DWChttp);
    if (iVar1 != 0) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(DWCNETCHECK_E_HTTPINITERR);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }

    iVar1 = OS_GetThreadPriority(OS_GetCurrentThread());
    DWC_Http_StartThread(DWChttp, iVar1 + -1);
    if ((DWChttp->thread).id != 0) {
      OS_JoinThread(&DWChttp->thread);
    }
    if (DWChttp->error == DWCHTTP_E_DNSERR) {
      DWCnetcheck->returncode = -1;
LAB_DWCi_Netcheck_Thread_DestroyHttp:
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(3);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    if (DWChttp->error != DWCHTTP_E_FINISH) goto LAB_DWCi_Netcheck_Thread_DestroyHttp;
    iVar1 = DWC_Http_ParseResult(DWChttp,0);
    if (iVar1 != 1) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(2);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }

    httpResult = DWC_Http_GetResult(DWChttp, "httpresult");
    httpStatusCode = atoi(httpResult);
    if (errno == 0x22) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Failed to get HTTP result code from received packet\n");
      DWCi_Netcheck_SetError(DWCNETCHECK_E_HTTPPARSEERR);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }

    if (httpStatusCode == 200) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : HTTP 200 Successed to get blacklist site\n");
      DWCconntestip = DWChttp->hostip;
    } else {
      if (httpStatusCode != 302) {
        DWC_Printf(DWC_REPORTFLAG_AUTH,"Netcheck : Not HTTP 200 nor HTTP 302 Network error\n");
        DWC_Http_Destroy(DWChttp);
        DWCi_Netcheck_SetError(DWCNETCHECK_E_NETUNAVAIL);
        goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
      }
      DWCishotspot = 1;
      if (DWCnetcheck->body_wayport != NULL) {
        DWCnetcheck->returncode = -6;

        DWC_Printf(DWC_REPORTFLAG_AUTH,"Netcheck : HTTP 302 Failed to get blacklist site twice\n");
        DWC_Http_Destroy(DWChttp);

        DWCnetcheckhttpparam.url         = DWCauthhttpparam.url;
        DWCnetcheckhttpparam.action      = DWCHTTP_POST;
        DWCnetcheckhttpparam.len_recvbuf = 0x200;
        DWCnetcheckhttpparam.timeout     = 20000;
        DWCnetcheckhttpparam.alloc       = DWCnetcheck->param.alloc;
        DWCnetcheckhttpparam.free        = DWCnetcheck->param.free;

        iVar1 = strcmp(DWCauthhttpparam.url, "https://nas.nintendowifi.net/ac");
        if (iVar1 != 0) {
          DWCnetcheckhttpparam.ignoreca = TRUE;
        }

        DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Posting NG message to NAS\n");

        iVar1 = DWC_Http_Create(DWChttp,&DWCnetcheckhttpparam);
        if (iVar1 != 0) {
          DWC_Http_Destroy(DWChttp);
          DWCi_Netcheck_SetError(1);
          goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
        }

        iVar1 = 1;
        DWC_Auth_GetCalInfoFromNVRAM(calInfoBuf);
        if ((iVar1 == 0) || (iVar1 = DWC_Auth_SetCalInfoToHttp(DWChttp,calInfoBuf,1), iVar1 == 0))
        {
          DWC_Http_Destroy(DWChttp);
          DWCi_Netcheck_SetError(8);
          goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
        }

        iVar1 = DWC_Http_Add_PostBase64Item(DWChttp, "action", "message", 7);
        if (iVar1 == DWCHTTP_E_NOERR) {
          sVar2 = strlen(DWCnetcheck->body_wayport);
          iVar1 = DWC_Http_Add_PostBase64Item(DWChttp, "HotSpotResponse", DWCnetcheck->body_wayport, sVar2);
          if (iVar1 == 0) {
            DWCnetcheck->param.free((int)"FREE DWCnetcheck->body_wayport",DWCnetcheck->body_wayport,0);
            DWCnetcheck->body_wayport = (char *)0x0;
            iVar1 = DWC_Http_FinishHeader(DWChttp);
            if (iVar1 != 0) {
              DWC_Http_Destroy(DWChttp);
              DWCi_Netcheck_SetError(DWCNETCHECK_E_HTTPINITERR);
              goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
            }
            iVar1 = OS_GetThreadPriority(OS_GetCurrentThread());
            DWC_Http_StartThread(DWChttp,iVar1 + -1);
            if ((DWChttp->thread).id != 0) {
              OS_JoinThread(&DWChttp->thread);
            }
            if (DWChttp->error == DWCHTTP_E_DNSERR) {
              DWCnetcheck->returncode = -1;
            } else if (DWChttp->error == DWCHTTP_E_FINISH) {
              DWC_Http_Destroy(DWChttp);
              DWCi_Netcheck_SetError(DWCNETCHECK_E_302TWICE);
              goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
            }
            DWC_Http_Destroy(DWChttp);
            DWCi_Netcheck_SetError(DWCNETCHECK_E_HTTPERR);
            goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
          }
        }
        DWC_Http_Destroy(DWChttp);
        DWCi_Netcheck_SetError(DWCNETCHECK_E_CANTADDPOSTITEM);
        goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
      }
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : HTTP 302 Failed to get blacklist site\n");
      local_294 = (DWChttp->rep).buffer;
      if (local_294 == (char *)0x0) {
        DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Failed to find HTTP body\n");
        DWC_Http_Destroy(DWChttp);
        DWCi_Netcheck_SetError(2);
        goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
      }
      sVar2 = strlen(local_294);
      pcVar3 = DWCnetcheck->param.alloc((int)"ALLOC DWCnetcheck->body_302",sVar2 + 1);
      DWCnetcheck->body_302 = pcVar3;
      if (DWCnetcheck->body_302 == (char *)0x0) {
        DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Failed to allocate memory to store 302 message\n");
        DWC_Http_Destroy(DWChttp);
        DWCi_Netcheck_SetError(4);
        goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
      }
      sVar2 = strlen(local_294);
      strncpy(DWCnetcheck->body_302,local_294,sVar2 + 1);
    }

    DWC_Http_Destroy(DWChttp);
    DWCi_BM_GetWiFiInfo(&DWC_netcheck_wifiinfo);
    DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Ensuring NAS user account existance\n");

    if(DWC_netcheck_wifiinfo.attestedUserId == 0) {
      DWCnetcheck->returncode = -3;
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : No authenticated accounts in this NDS. Entering new account create procedure\n");
      DWCnetcheckauthparam.ingamesn[0] = 0;
      DWCnetcheckauthparam.gsbrcd[0] = '\0';
      DWCnetcheckauthparam.alloc = DWCnetcheck->param.alloc;
      DWCnetcheckauthparam.free = DWCnetcheck->param.free;
      iVar4 = DWC_Auth_Create(&DWCnetcheckauthparam, DWChttp);
      if (iVar4 != 0) {
        DWCi_Netcheck_SetError(DWCNETCHECK_E_AUTHINITERR);
        goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
      }
      DWC_Auth_Join();
      iVar4 = DWC_Auth_GetError();
      if (iVar4 != 0x15) {
        iVar1 = DWC_Auth_GetError();
        if (iVar1 == 9) {
          DWCnetcheck->returncode = -1;
        } else {
          //DWC_Auth_GetResult(local_1e4);
          //if ((DWCallow302 == 1) &&
          //   ((local_1e4[0] == -0x5b06 || (iVar1 = DWC_Auth_GetError(), iVar1 == 0xb)))) {
          //  DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : HTTP 302 or connection error and network may be AVAILABLE\n");
          //  DWCnetcheck->returncode = 0;
          //  DWC_Auth_Destroy();
          //  DWCi_Netcheck_SetError(0xb);
          //  goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
          //}
          DWCnetcheck->returncode = local_1e4[0];
        }
        DWC_Auth_Destroy();
        DWCi_Netcheck_SetError(6);
        goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
      }
      DWC_Auth_Destroy();
    }
    DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Successed to ensure NAS user account existance\n");


    if (/*iVar1*/httpStatusCode == 200) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : HTTP 200 Network and NAS account are AVAILABLE\n");
      DWCnetcheck->returncode = 0;
      DWCi_Netcheck_SetError(DWCNETCHECK_E_NETAVAIL);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }

    DWCnetcheck->returncode = -4;

    DWCnetcheckhttpparam.url         = DWCauthhttpparam.url;
    DWCnetcheckhttpparam.action      = DWCHTTP_E_NOERR;
    DWCnetcheckhttpparam.len_recvbuf = 0x1000;
    DWCnetcheckhttpparam.timeout     = 40000;
    DWCnetcheckhttpparam.alloc       = DWCnetcheck->param.alloc;
    DWCnetcheckhttpparam.free        = DWCnetcheck->param.free;
    iVar1 = strcmp(DWCauthhttpparam.url, "https://nas.nintendowifi.net/ac");
    if (iVar1 != 0) {
      DWCnetcheckhttpparam.ignoreca = 1;
    }
    DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Posting HTTP 302 message to NAS\n");
    iVar1 = DWC_Http_Create(DWChttp,&DWCnetcheckhttpparam);
    if (iVar1 != 0) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(1);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar1 = 1;
    DWC_Auth_GetCalInfoFromNVRAM(calInfoBuf);
    if ((iVar1 == 0) || (iVar1 = DWC_Auth_SetCalInfoToHttp(DWChttp,calInfoBuf,1), iVar1 == 0)) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(8);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar1 = DWC_Http_Add_PostBase64Item(DWChttp, "action", "parse", 5);
    if (iVar1 != 0) {
LAB_00011554:
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(8);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    sVar2 = strlen(DWCnetcheck->body_302);
    iVar1 = DWC_Http_Add_PostBase64Item(DWChttp, "HTML", DWCnetcheck->body_302, sVar2);
    if (iVar1 != 0) goto LAB_00011554;
    DWCnetcheck->param.free((int)"FREE DWCnetcheck->body_302",DWCnetcheck->body_302,0);
    DWCnetcheck->body_302 = (char *)0x0;
    iVar1 = DWC_Http_FinishHeader(DWChttp);
    if (iVar1 != 0) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(1);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar1 = OS_GetThreadPriority(OS_GetCurrentThread());
    DWC_Http_StartThread(DWChttp,iVar1 + -1);
    if ((DWChttp->thread).id != 0) {
      OS_JoinThread(&DWChttp->thread);
    }
    _Var5 = DWChttp->error;
    if (_Var5 == DWCHTTP_E_DNSERR) {
      DWCnetcheck->returncode = -1;
LAB_000116a4:
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(3);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    if (_Var5 == DWCHTTP_E_CONERR) {
      DWC_Http_Destroy(DWChttp);
      if (DWCallow302 == 1) {
        DWC_Printf(DWC_REPORTFLAG_AUTH,"Netcheck : HTTP connection error and network may be AVAILABLE\n");
        DWCnetcheck->returncode = 0;
        DWCi_Netcheck_SetError(0xb);
      }
      else {
        DWCi_Netcheck_SetError(3);
      }
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    if (_Var5 != DWCHTTP_E_FINISH) goto LAB_000116a4;
    iVar1 = DWC_Http_ParseResult(DWChttp, 0);
    if (iVar1 != 1) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(2);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    httpResult = DWC_Http_GetResult(DWChttp, "httpresult");
    httpStatusCode = atoi(httpResult);
    if (errno == 0x22) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(2);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    if (httpStatusCode != 200) {
      DWC_Http_Destroy(DWChttp);
      if ((DWCallow302 == 1) && (httpStatusCode == 302)) {
        DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : HTTP 302 and network may be AVAILABLE\n");
        DWCnetcheck->returncode = 0;
        DWCi_Netcheck_SetError(DWCNETCHECK_E_NETAVAIL);
      } else {
        DWCi_Netcheck_SetError(2);
      }
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar1 = DWC_Http_GetBase64DecodedResult(DWChttp, "returncd", acStack_29c, 4);
    if (iVar1 < 1) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(9);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar1 = atoi(acStack_29c);
    if (errno == 0x22) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(9);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    if ((DWCallow302 == 1) && (iVar1 == 0x72)) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : NAS return code 114 and network may be AVAILABLE\n");
      DWCnetcheck->returncode = 0;
      DWCi_Netcheck_SetError(0xb);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    if (99 < iVar1) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Auth server returns errorcode %d\n", iVar1);
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(6);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar1 = DWC_Http_GetBase64DecodedResult(DWChttp,"url",0,0);
    if (iVar1 < 1) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(9);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar4 = DWC_Http_GetBase64DecodedResult(DWChttp,"data",0,0);
    if (iVar4 < 1) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(9);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar6 = DWC_Http_GetBase64DecodedResult(DWChttp,"wait",0,0);
    url = DWCnetcheck->param.alloc((int)"ALLOC url",iVar1 + 1);
    if (url == NULL) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(4);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    ptr_00 = DWCnetcheck->param.alloc((int)"ALLOC data_len",iVar4 + 1);
    if (ptr_00 == NULL) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(4);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    if ((0 < iVar6) && (__nptr = DWCnetcheck->param.alloc((int)"ALLOC wait_len",iVar6 + 1), __nptr == NULL)) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(4);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar1 = DWC_Http_GetBase64DecodedResult(DWChttp, "url", url, iVar1 + 1);
    if (iVar1 < 0) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(9);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    url[iVar1] = '\0';
    iVar1 = DWC_Http_GetBase64DecodedResult(DWChttp, "data", ptr_00, iVar4 + 1);
    if (iVar1 < 0) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(9);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    *(undefined *)((int)ptr_00 + iVar1) = 0;
    local_2a8 = 0;
    if (0 < iVar6) {
      iVar1 = DWC_Http_GetBase64DecodedResult(DWChttp, "wait", __nptr, iVar6 + 1);
      if (iVar1 < 0) {
        DWC_Http_Destroy(DWChttp);
        DWCi_Netcheck_SetError(9);
        goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
      }
      __nptr[iVar1] = '\0';
      local_2a8 = atoi(__nptr);
      if (errno == 0x22) {
        DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Failed to get wait time from received packet\n");
        DWC_Http_Destroy(DWChttp);
        DWCi_Netcheck_SetError(9);
        goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
      }
      local_2a8 = local_2a8 * 1000;
      if (180000 < local_2a8) {
        local_2a8 = 180000;
      }
    }
    DWC_Http_Destroy(DWChttp);
    DWCnetcheck->returncode = -5;
    DWCnetcheckhttpparam.action = DWCHTTP_E_NOERR;
    DWCnetcheckhttpparam.len_recvbuf = 0x1000;
    DWCnetcheckhttpparam.timeout = 120000;
    DWCnetcheckhttpparam.url = url;
    DWCnetcheckhttpparam.alloc = p_Var7;
    DWCnetcheckhttpparam.free = p_Var8;
    DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Posting authorize information to the Hotspot auth server\n");
    iVar1 = DWC_Http_Create(DWChttp,&DWCnetcheckhttpparam);
    if (iVar1 != 0) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(1);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar1 = DWC_Http_Add_Body(DWChttp,ptr_00);
    if (iVar1 != 0) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(8);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    iVar1 = DWC_Http_FinishHeader(DWChttp);
    if (iVar1 != 0) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(1);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    //OS_GetCurrentThread();
    //iVar1 = OS_GetThreadPriority();
    iVar1 = OS_GetThreadPriority(OS_GetCurrentThread());
    DWC_Http_StartThread(DWChttp,iVar1 + -1);
    if ((DWChttp->thread).id != 0) {
      OS_JoinThread(&DWChttp->thread);
    }
    if (DWChttp->error == DWCHTTP_E_DNSERR) {
      DWCnetcheck->returncode = -1;
LAB_00011d50:
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(3);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    if (DWChttp->error != DWCHTTP_E_FINISH) goto LAB_00011d50;
    iVar1 = DWC_Http_ParseResult(DWChttp,1);
    if (iVar1 != 1) {
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(2);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Received response from Hotspot auth server\n\n");
    local_298 = (DWChttp->rep).buffer;
    if (local_298 == (char *)0x0) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Failed to find HTTP body\n");
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(2);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    sVar2 = strlen(local_298);
    pcVar3 = DWCnetcheck->param.alloc((int)"ALLOC DWCnetcheck->body_wayport",sVar2 + 1);
    DWCnetcheck->body_wayport = pcVar3;
    if (DWCnetcheck->body_wayport == (char *)0x0) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "Netcheck : Failed to allocate memory to store wayport auth response\n");
      DWC_Http_Destroy(DWChttp);
      DWCi_Netcheck_SetError(4);
      goto LAB_DWCi_Netcheck_Thread_CheckAndFreePtrs;
    }
    sVar2 = strlen(local_298);
    strncpy(DWCnetcheck->body_wayport,local_298,sVar2 + 1);
    DWC_Http_Destroy(DWChttp);
    OS_Sleep(local_2a8);
  } while( TRUE );
}

void DWCi_Netcheck_SetError(DWCNetcheckError aErr)
{
  OS_LockMutex(&DWCnetcheck->mutex);
  DWCnetcheck->error = aErr;
  OS_UnlockMutex(&DWCnetcheck->mutex);
  return;
}

BOOL DWC_SimpleNetcheckAsync(int timeout)
{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  longlong lVar4;
  longlong lVar5;
  
  if ((DWCsimplecheckwork == (DWCSimpleNetcheck *)0x0) /*&& (DWCconntestip != 0)*/) {
    //DWCsimplecheckwork = (DWCSimpleNetcheck *)DWC_Alloc(0,0x48,param_3,param_4,param_4);
    if (DWCsimplecheckwork == (DWCSimpleNetcheck *)0x0) {
      uVar1 = 0;
    } else {
      MI_CpuClear8(DWCsimplecheckwork,0x48);
      DWCsimplecheckwork->sock = -1;
      iVar2 = SOC_Socket(2,1,0);
      DWCsimplecheckwork->sock = iVar2;
      if (DWCsimplecheckwork->sock < 0) {
        //DWC_Free(0,DWCsimplecheckwork,0);
        DWCsimplecheckwork = (DWCSimpleNetcheck *)0x0;
        uVar1 = 0;
      } else {
        uVar3 = SOC_Fcntl(DWCsimplecheckwork->sock,3,0);
        SOC_Fcntl(DWCsimplecheckwork->sock,4,uVar3 | 4);
        (DWCsimplecheckwork->socaddr).len = '\b';
        (DWCsimplecheckwork->socaddr).family = '\x02';
        (DWCsimplecheckwork->socaddr).port = 0x5000;
        //(DWCsimplecheckwork->socaddr).addr.addr =
        //     DWCconntestip << 0x18 |
        //     (DWCconntestip & 0xff00) << 8 | DWCconntestip >> 0x18 | DWCconntestip >> 8 & 0xff00;
        iVar2 = SOC_Connect(DWCsimplecheckwork->sock,&DWCsimplecheckwork->socaddr);
        if ((iVar2 < 0) && (iVar2 != -0x1a)) {
          DWCsimplecheckwork->result = DWCHTTP_E_CONERR;
          DWCsimplecheckwork->stage = DWCHTTP_E_CONERR;
          SOC_Shutdown(DWCsimplecheckwork->sock,2);
          SOC_Close(DWCsimplecheckwork->sock);
          uVar1 = 1;
        } else {
          DWCsimplecheckwork->stage = DWCHTTP_E_MEMERR;
          //if ((int)param_1 < 1) {
          //  lVar4 = OS_GetTick();
          //  DWCsimplecheckwork->finishtick = lVar4 + 0x17f898;
          //}
          //else {
          //  lVar4 = OS_GetTick();
            //lVar5 = _ll_udiv((int)((ulonglong)param_1 * 0x82ea),
            //                 ((int)param_1 >> 0x1f) * 0x82ea +
            //                 (int)((ulonglong)param_1 * 0x82ea >> 0x20),0x40,0);
           // DWCsimplecheckwork->finishtick = lVar4 + lVar5;
          }
          uVar1 = 1;
        }
      }
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

DWCSimpleNetcheckResult DWC_SimpleNetcheckProcess(void)
{
  DWCSimpleNetcheckResult _Var1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  BOOL bVar5;
  undefined8 uVar6;
  
  if ((DWCsimplecheckwork == (DWCSimpleNetcheck *)0x0) ||
     (DWCsimplecheckwork->stage == DWCHTTP_E_NOERR)) {
    _Var1 = DWCHTTP_E_SENDTOUT;
  }
  else {
    if ((DWCsimplecheckwork->result == DWCHTTP_E_NOERR) &&
       (*(int *)((int)&DWCsimplecheckwork->finishtick + 4) != 0 ||
        *(int *)&DWCsimplecheckwork->finishtick != 0)) {
      uVar6 = OS_GetTick();
      uVar3 = (uint)((ulonglong)uVar6 >> 0x20);
      uVar4 = *(uint *)((int)&DWCsimplecheckwork->finishtick + 4);
      bVar5 = uVar3 <= uVar4;
      if (uVar4 == uVar3) {
        bVar5 = (uint)uVar6 <= *(uint *)&DWCsimplecheckwork->finishtick;
      }
      if (!bVar5) {
        DWCsimplecheckwork->result = DWCHTTP_E_DNSERR;
        DWCsimplecheckwork->stage = DWCHTTP_E_CONERR;
        SOC_Shutdown(DWCsimplecheckwork->sock,2);
        SOC_Close(DWCsimplecheckwork->sock);
      }
    }
    _Var1 = DWCsimplecheckwork->stage;
    if (_Var1 == DWCHTTP_E_MEMERR) {
      iVar2 = SOC_Connect(DWCsimplecheckwork->sock,&DWCsimplecheckwork->socaddr);
      if ((iVar2 < 0) && (iVar2 != -0x1a)) {
        DWCsimplecheckwork->result = DWCHTTP_E_CONERR;
        DWCsimplecheckwork->stage = DWCHTTP_E_CONERR;
        SOC_Shutdown(DWCsimplecheckwork->sock,2);
        SOC_Close(DWCsimplecheckwork->sock);
        _Var1 = DWCHTTP_E_NOERR;
      }
      else {
        DWCi_SimpleNetcheckSendRequest();
        _Var1 = DWCHTTP_E_NOERR;
      }
    }
    else if (_Var1 == DWCHTTP_E_DNSERR) {
      DWCi_SimpleNetcheckReceiveReply();
      _Var1 = DWCHTTP_E_NOERR;
    }
    else if (_Var1 == DWCHTTP_E_CONERR) {
      _Var1 = DWCsimplecheckwork->result;
      //DWC_Free(0,DWCsimplecheckwork,0);
      DWCsimplecheckwork = (DWCSimpleNetcheck *)0x0;
    }
    else {
      DWCsimplecheckwork->result = DWCHTTP_E_CONERR;
      DWCsimplecheckwork->stage = DWCHTTP_E_CONERR;
      SOC_Shutdown(DWCsimplecheckwork->sock,2);
      SOC_Close(DWCsimplecheckwork->sock);
      _Var1 = DWCHTTP_E_NOERR;
    }
  }
  return _Var1;
}

void DWCi_SimpleNetcheckSendRequest(void)
{
  size_t sVar1;
  int iVar2;
  
  //sVar1 = strlen(_13771);
  //iVar2 = SOC_Write(DWCsimplecheckwork->sock,_13771,sVar1);
  if (iVar2 != -0x38) {
    if (iVar2 < 1) {
      DWCsimplecheckwork->result = DWCHTTP_E_DNSERR;
      DWCsimplecheckwork->stage = DWCHTTP_E_CONERR;
      SOC_Shutdown(DWCsimplecheckwork->sock,2);
      SOC_Close(DWCsimplecheckwork->sock);
    }
    else {
      DWCsimplecheckwork->stage = DWCHTTP_E_DNSERR;
      DWCsimplecheckwork->recvindex = 0;
    }
  }
  return;
}

void DWCi_SimpleNetcheckReceiveReply(void)
{
  int iVar1;
  undefined4 in_r3;
  
  //iVar1 = SOC_Read(DWCsimplecheckwork->sock,
  //                 DWCsimplecheckwork->recvbuf + DWCsimplecheckwork->recvindex,
  //                 0x20 - DWCsimplecheckwork->recvindex,in_r3,in_r3);
  if (iVar1 != -6) {
    if (iVar1 < 0) {
      DWCsimplecheckwork->result = DWCHTTP_E_DNSERR;
      DWCsimplecheckwork->stage = DWCHTTP_E_CONERR;
      SOC_Shutdown(DWCsimplecheckwork->sock,2);
      SOC_Close(DWCsimplecheckwork->sock);
    }
    else {
      DWCsimplecheckwork->recvindex = DWCsimplecheckwork->recvindex + iVar1;
      if ((iVar1 == 0) || (0x1f < DWCsimplecheckwork->recvindex)) {
        for (iVar1 = 0; (DWCsimplecheckwork->recvbuf[iVar1] != ' ' && (iVar1 < 0x20));
            iVar1 = iVar1 + 1) {
        }
        if (iVar1 + 1 < 0x20) {
          //iVar1 = strncmp(DWCsimplecheckwork->recvbuf + iVar1 + 1,(char *)&_13787,3);
          if (iVar1 == 0) {
            DWCsimplecheckwork->result = DWCHTTP_E_MEMERR;
          }
          else {
            DWCsimplecheckwork->result = DWCHTTP_E_DNSERR;
          }
        }
        else {
          DWCsimplecheckwork->result = DWCHTTP_E_DNSERR;
        }
        DWCsimplecheckwork->stage = DWCHTTP_E_CONERR;
        SOC_Shutdown(DWCsimplecheckwork->sock,2);
        SOC_Close(DWCsimplecheckwork->sock);
      }
    }
  }
  return;
}


