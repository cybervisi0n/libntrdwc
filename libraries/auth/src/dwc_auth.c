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

#include "dwc_serverurl.h"

#ifdef SDK_BUILD_LINUX
#include <errno.h>
#include <wchar.h>
#endif

#ifndef SDK_BUILD_ARM
	BOOL DWCi_AUTH_UpDateWiFiID(DWCWiFiInfo* wifiinfo, void* work);
#endif

#define DWCAUTH_USERAGENT	"Nitro WiFi SDK/1.0"
#define DWCAUTH_SDKVER		"001000"

#define DWCAUTH_RETRYWAIT	5000

DWCHttpParam DWCauthhttpparam = {
	DWC_AUTH_NAS_URL,
	DWCHTTP_POST,
	4096,
	NULL,
	NULL,
	FALSE,
	20000
};

DWCAuth		*DWCauth = NULL;
DWCWiFiInfo	DWC_auth_wifiinfo;
static BOOL DWCishotspot = FALSE;
static BOOL DWCiswfcutil = FALSE;
DWCAuthIngamesnCheckResult DWCauthingamesncheckresult;

char *DWCautherrstr[] = {
	"DWCAUTH_E_NOERR",
	"DWCAUTH_E_BMINITERR",
	"DWCAUTH_E_MEMERR",
	"DWCAUTH_E_ACERR",
	"DWCAUTH_E_HTTPINITERR",
	"DWCAUTH_E_RTCERR",
	"DWCAUTH_E_INVALIDPARAM",
	"DWCAUTH_E_CANTADDHTTPHEADER",
	"DWCAUTH_E_CANTADDPOSTITEM",
	"DWCAUTH_E_DNSERR",
	"DWCAUTH_E_CANTSTARTHTTPTHREAD",
	"DWCAUTH_E_HTTPPARSEERR",
	"DWCAUTH_E_HTTPERR",
	"DWCAUTH_E_NASPARSEERR",
	"DWCAUTH_E_BMERR",
	"DWCAUTH_E_NASALREADYEXIST",
	"DWCAUTH_E_NASACCTEXPIRE",
	"DWCAUTH_E_NASERR",
	"DWCAUTH_E_TOOMANYRETRY",
	"DWCAUTH_E_ABORT",
	"DWCAUTH_E_FINISH",
	"DWCAUTH_E_NOTINITIALIZED",
	"DWCAUTH_E_MAX"
};

void DWCi_Auth_SetError(DWCAuthError error);

void DWC_Auth_SetCustomNas(const char *nasaddr) {
	DWCauthhttpparam.url = nasaddr;
}

DWCAuthError DWC_Auth_Create(DWCAuthParam *param, void *http)
{
	DWCAuth *temp;
	

	if(DWCauth != NULL) {
		DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : DWCauth already allocated\n");
		return DWCAUTH_E_MEMERR;		
	}
	
	temp = param->alloc((unsigned long)"DWCAuth", sizeof(DWCAuth));
	if(temp == NULL) {
		DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Can't allocate memory for DWCauth\n");
		return DWCAUTH_E_MEMERR;		
	}
	
	DWCauth = temp;
	

	MI_CpuFill8(DWCauth, 0x00, sizeof(DWCAuth));
	

	DWCauth->http = (DWCHttp *)http;
		

	MI_CpuFill8(&DWCauth->result, 0x00, sizeof(DWCAuthResult));
	DWCauth->result.returncode = -1;
	

	DWCauth->param = *param;
	DWCauth->param.ingamesn[20]	= 0x00;
	DWCauth->param.gsbrcd[11]	= 0x00;	
	

	DWCauthhttpparam.alloc			= param->alloc;
	DWCauthhttpparam.free			= param->free;
	
	DWCauth->error = DWCi_Auth_Start(TRUE);
	if(DWCauth->error != DWCAUTH_E_NOERR)
		return DWCauth->error;
	

	DWCi_Auth_StartThread();
	
	return DWCAUTH_E_NOERR;
}

DWCAuthError DWC_Auth_Create_WFCUtil(DWCAuthParam *param, void *http)
{
  DWCAuthError theError;

  theError = DWC_Auth_Create(param, http);
  DWCiswfcutil = TRUE;
  return theError;
}

void DWCi_Auth_StartThread(void)
{

	OS_InitMutex(&DWCauth->mutex);
	DWCauth->abort = FALSE;
	

	if(DWCauth->thread.id != 0 ) {

		if(OS_IsThreadTerminated(&DWCauth->thread) == FALSE) {
			DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Auth thread already running\n");		
			return;
		}
	}
    OS_CreateThread(&DWCauth->thread, DWCi_Auth_Thread, (void *)&DWCauth, DWCauth->stack+sizeof(DWCauth->stack), sizeof(DWCauth->stack), 16);
    OS_WakeupThreadDirect(&DWCauth->thread);
}

void DWC_Auth_Abort(void)
{
	if(DWCauth == NULL)
		return;
	
	DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Aborting\n");
	
	DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Locking mutex\n");
	OS_LockMutex(&DWCauth->mutex);
	DWCauth->abort = TRUE;
	OS_UnlockMutex(&DWCauth->mutex);
	DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Freed mutex\n");
	

	if(DWCauth->http != NULL) {
		DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Aborting Http\n");
		DWC_Http_Abort(DWCauth->http);
		DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Aborting Http finished\n");		
	}
	
	DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Waiting for Auth thread finish\n");
	if(DWCauth->thread.id != 0)
		OS_JoinThread(&DWCauth->thread);
	
	DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Aborted\n");
}

void DWC_Auth_Destroy(void)
{
	if(DWCauth == NULL)
		return;
	

	if(DWCauth->http != NULL)
		DWC_Http_Destroy(DWCauth->http);
	

	DWCauth->param.free((unsigned long)"DWCauth", DWCauth, 0);
	DWCauth = NULL;
	
	return;
}

void DWC_Auth_Join(void)
{

	if(DWCauth->thread.id != 0)
		OS_JoinThread(&(DWCauth->thread));
}

DWCAuthError DWC_Auth_GetError(void)
{
	DWCAuthError error;
	
	if(DWCauth == NULL)
		return DWCAUTH_E_NOTINITIALIZED;
	
	OS_LockMutex(&DWCauth->mutex);
	error = DWCauth->error;
	OS_UnlockMutex(&DWCauth->mutex);
	
	return error;
}

void DWC_Auth_GetResult(DWCAuthResult *result)
{
	if(DWCauth == NULL)
		MI_CpuFill8(result, 0x00, sizeof(DWCAuthResult));
	
	MI_CpuCopy8(&DWCauth->result, result, sizeof(DWCAuthResult));
	if(result->returncode >= 0) {
		if(result->returncode < 20000 || result->returncode >= 30000)
			result->returncode = -20998;
	}
	else {
		if(result->returncode > -20000 || result->returncode <= -30000)
			result->returncode = -20998;
	}
}

undefined4 DWCi_Auth_Start(BOOL getwifiinfofromflash)
{

	if(strcmp(DWCauthhttpparam.url, DWC_AUTH_NAS_URL) != 0)
		DWCauthhttpparam.ignoreca = TRUE;
	

	if(DWC_Http_Create(DWCauth->http, &DWCauthhttpparam) != DWCHTTP_E_NOERR)
		return DWCAUTH_E_HTTPINITERR;
	

	if(getwifiinfofromflash == TRUE)
		DWCi_BM_GetWiFiInfo(&DWC_auth_wifiinfo);
		

	DWCauth->error = DWCi_Auth_Prepare_FirstPost(DWCauth->http, DWCauth->param.gsbrcd, &DWCauth->param, FALSE);
	if(DWCauth->error != DWCAUTH_E_NOERR) {
		DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Failed to initialize HTTP POST packet\n");		
		return DWCAUTH_E_HTTPINITERR;
	}
	

	if(DWC_Http_FinishHeader(DWCauth->http) != DWCHTTP_E_NOERR) {
		DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Failed to finalize HTTP POST packet\n");		
		return DWCAUTH_E_HTTPINITERR;
	}
	

	DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Start HTTP/HTTPS communication\n");

	DWC_Http_StartThread(DWCauth->http, 0);

	return DWCAUTH_E_NOERR;
}

void DWCi_Auth_Thread(void *arg)
{
#pragma unused(arg)
	int retrycount = 0;
	OSTick startwait;
	DWCAuthError result_httpparse;
	BOOL getwifiinfofromflash;
	

	for(;;) {
		if(DWCauth->http->thread.id != 0)
			OS_JoinThread(&(DWCauth->http->thread));
		

		if(DWCauth->http->error != DWCHTTP_E_FINISH) {
			DWCauth->result.returncode = -20100;
			
			DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Error %s (1)\n", DWCautherrstr[DWCauth->error]);			

			if(DWCauth->http->error == DWCHTTP_E_ABORT) {
				DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Aborting\n");			
				DWCi_Auth_SetError(DWCAUTH_E_ABORT);
				DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Seterror finished\n");
				return;
			}
						
			if(retrycount > 2) {
				DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : TOOMANYRETRY\n");
				if(DWCauth->http->error == DWCHTTP_E_DNSERR)
					DWCi_Auth_SetError(DWCAUTH_E_DNSERR);
				else
					DWCi_Auth_SetError(DWCAUTH_E_HTTPERR);
				DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Seterror finished\n");
				return;		
			}
			

			retrycount++;
			getwifiinfofromflash = TRUE;
			goto retry;
		}
		

		DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Parsing HTTP reply\n");			
		result_httpparse = DWCi_Auth_ParseHttp();
		DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Parsing finished\n");			
		
		switch(result_httpparse) {
		case DWCAUTH_E_FINISH:

			DWCi_Auth_SetError(DWCAUTH_E_FINISH);
			return;
		case DWCAUTH_E_NASALREADYEXIST:

			DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Error %s (2)\n", DWCautherrstr[result_httpparse]);			

			if(retrycount >= 2) {
				DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : TOOMANYRETRY\n");
				DWCi_Auth_SetError(DWCAUTH_E_NASALREADYEXIST);
				DWCauth->result.returncode = -DWCauth->result.returncode;
				return;
			}
			
			retrycount++;
			getwifiinfofromflash = FALSE;
			goto retry;
		default:

			DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Error %s (3)\n", DWCautherrstr[result_httpparse]);			
			if(retrycount >= 2) {
				DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : TOOMANYRETRY\n");
				DWCauth->result.returncode = -DWCauth->result.returncode;
				DWCi_Auth_SetError(result_httpparse);
				return;				
			}
			
			retrycount++;
			getwifiinfofromflash = TRUE;
			goto retry;
		}
		

retry:
		startwait = OS_GetTick();

		DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Wait 5 seconds for retry\n");
		#ifdef SDK_PORT
		OS_Sleep(5000);
		#else
		while(OS_TicksToMilliSeconds(OS_GetTick()-startwait) < DWCAUTH_RETRYWAIT) {
			OS_LockMutex(&DWCauth->mutex);
			if(DWCauth->abort == TRUE) {
				DWCauth->result.returncode = -20100;
				OS_UnlockMutex(&DWCauth->mutex);
				DWCi_Auth_SetError(DWCAUTH_E_ABORT);
				return;
			}
			OS_UnlockMutex(&DWCauth->mutex);
			OS_Sleep(5000);
		}
		#endif
		DWC_Http_Destroy(DWCauth->http);
		OS_LockMutex(&DWCauth->mutex);
		DWCauth->error = DWCi_Auth_Start(getwifiinfofromflash);
		if(DWCauth->error != DWCAUTH_E_NOERR) {
			DWCauth->result.returncode = -20100;
			OS_UnlockMutex(&DWCauth->mutex);
			return;
		}
		OS_UnlockMutex(&DWCauth->mutex);
	}
}

DWCAuthError DWCi_Auth_ParseHttp(void)
{
  int iVar1;
  void *pvVar2;
  DWCAuthFree freeFunc;
  DWCAuthAlloc allocFunc;
  DWCHttp *http;
  
  allocFunc = (DWCauth->param).alloc;
  freeFunc = (DWCauth->param).free;
  http = DWCauth->http;
  DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : HTTP POST finished\n");
  iVar1 = DWC_Http_ParseResult(http,0);
  if (iVar1 != 1) {
    (DWCauth->result).returncode = 20100;
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Unparseable HTTP message from NAS error - %d\n", DWCauth->result.returncode);
    return DWCAUTH_E_NASPARSEERR;
  }
  iVar1 = DWCi_Auth_FillResult();
  if (iVar1 != 0) {
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Non 200 HTTP message(or invalid NAS reply) from NAS error - %d\n", DWCauth->result.returncode);
    return DWCAUTH_E_NASPARSEERR;
  }
  if ((DWCauth->result).returncode < 20100) {
    if ((DWCauth->result).returncode == 20002) {
      pvVar2 = (*allocFunc)((int)"ALLOC bmwork", 0x71f);
      if (pvVar2 == NULL) {
        (DWCauth->result).returncode = 20100;
        return 2;
      }
      #ifdef SDK_BUILD_ARM
      iVar1 = DWCi_AUTH_UpDateWiFiID(&DWC_auth_wifiinfo,(int)pvVar2 + 0x1fU & 0xffffffe0);
      #else
      iVar1 = DWCi_AUTH_UpDateWiFiID(&DWC_auth_wifiinfo, (void*)((u64)pvVar2 + 0x1fU));
      #endif
      if (iVar1 != 1) {
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : UpdateWiFiID error\n");
        (*freeFunc)((int)"FREE bmwork", pvVar2, 0);
        (DWCauth->result).returncode = 20100;
        return DWCAUTH_E_BMERR;
      }
      (*freeFunc)((int)"FREE bmwork", pvVar2, 0);
    }
    return DWCAUTH_E_FINISH;
  }
  iVar1 = DWCauth->result.returncode;
  if (iVar1 == 20104) {
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns DUPE_USER error - %d\n", DWCauth->result.returncode);

    if (iVar1 == 0) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : RemakeWiFiID Error\n");
      DWCauth->result.returncode = 20104;
      return DWCAUTH_E_NASALREADYEXIST;
    }
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Local account re-created. Retry to create NAS account again\n");
    DWCauth->result.returncode = 20104;
    return DWCAUTH_E_NASALREADYEXIST;
  }
  if (iVar1 == 20108) {
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns USER_DELETED error - %d\n", DWCauth->result.returncode);
    pvVar2 = (*allocFunc)((int)"ALLOC bmwork", 0x700);
    if (pvVar2 == NULL) {
      (DWCauth->result).returncode = 20108;
      return DWCAUTH_E_NASACCTEXPIRE;
    }

    if (iVar1 != 1) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : MakeWiFiID Error\n");
      (*freeFunc)((int)"FREE bmwork", pvVar2, 0);
      DWCauth->result.returncode = 20108;
      return DWCAUTH_E_NASACCTEXPIRE;
    }
    (*freeFunc)((int)"FREE bmwork", pvVar2, 0);
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Local account re-created and flashed\n");
    (DWCauth->result).returncode = 20108;
    return DWCAUTH_E_NASACCTEXPIRE;
  }

  iVar1 = DWCauth->result.returncode;
  if (iVar1 < 20302) {
    if (20300 < iVar1) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns FEATURE_NOT_SUPPOTED error\n");
      goto LAB_000117cc;
    }
    if (iVar1 < 20201) {
      if (20199 < iVar1) {
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns BAD_REQUEST_PROCSSING error\n");
        goto LAB_000117cc;
      }
      switch(iVar1) {
      case 20102:
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns BANNED_USER error\n");
        break;
      case 20103:
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns INVALID_USERID_MAC error\n");
        break;
      case 20104:
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns DUPE_USER error\n");
        break;
      case 20105:
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns UNKNOWN_ID error\n");
        break;
      case 20106:
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns ID_LIMIT_EXC error\n");
        break;
      case 20107:
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns UNKNOWN_GAME error\n");
        break;
      case 20108:
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns USER_DELETED error\n");
        break;
      case 20109:
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns BAD_CAL_DATA error\n");
        break;
      case 20110:
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns SERVICE_EXP error\n");
        break;
      default:
        goto switchD_00011658_caseD_9;
      }
      goto LAB_000117cc;
    }
    if (iVar1 == 20204) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns USER_NOT_FOUND error\n");
      goto LAB_000117cc;
    }
  }
  else if (iVar1 < 20503) {
    if (0x5015 < iVar1) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns SERVER_LOGIN_ERR error\n");
      goto LAB_000117cc;
    }
    if (iVar1 == 20501) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns UNSPEC_SERV_ERR error\n");
      goto LAB_000117cc;
    }
  }
  else if (iVar1 == 20503) {
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns SERVER_RADIUS_ERR error\n");
    goto LAB_000117cc;
  }
switchD_00011658_caseD_9:
  DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : Server returns UNKNOWN_ERR error\n");
LAB_000117cc:
  DWC_Printf(DWC_REPORTFLAG_AUTH, "Error Code = %d\n", DWCauth->result.returncode);
  return DWCAUTH_E_NASERR;
}

u32 DWC_Auth_SetNasTimeDiff(char *aTime);

DWCAuthError DWCi_Auth_FillResult(void)
{
  char *__nptr;
  int iVar1;
  undefined4 uVar2;
  long lVar3;
  size_t sVar4;
  undefined4 in_r3;
  char *local_10;
  undefined4 uStack_c;
  
  local_10 = NULL;
  uStack_c = in_r3;
  local_10 = DWC_Http_GetResult(DWCauth->http, "Date");
  if (local_10 != NULL) {
    DWC_Auth_SetNasTimeDiff(local_10);
  }
  local_10 = NULL;

  __nptr = DWC_Http_GetResult(DWCauth->http, "httpresult");
  iVar1 = atoi(__nptr);
  if (errno == 0x22) {
    (DWCauth->result).returncode = 20101;
    uVar2 = 0xc;
  }
  if(0) {}
  else if (iVar1 == 200) {
    iVar1 = DWC_Http_GetBase64DecodedResult(DWCauth->http, "returncd", DWCauth->result.retcode, 4);
    if (iVar1 < 1) {
      DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : NO NAS RESPONSE CODE FOUND\n");
      DWCauth->result.returncode = 20101;
      uVar2 = 0xe;
    } else {
      lVar3 = strtol(DWCauth->result.retcode, &local_10, 10);
      sVar4 = strlen(DWCauth->result.retcode);
      if (local_10 == (DWCauth->result).retcode + sVar4) {
        (DWCauth->result).returncode = lVar3 + 20000;
        if (lVar3 < 100) {
          DWCauth->result.token[0] = '\0';
          DWCauth->result.locator[0] = '\0';
          DWCauth->result.challenge[0] = '\0';
          DWCauth->result.datetime[0] = '\0';
          DWCauth->result.cookie[0] = '\0';
          DWC_Http_GetBase64DecodedResult(DWCauth->http, "token", DWCauth->result.token, 0x12d);
          DWC_Http_GetBase64DecodedResult(DWCauth->http, "locator", DWCauth->result.locator, 0x33);
          DWC_Http_GetBase64DecodedResult(DWCauth->http, "challenge", DWCauth->result.challenge, 9);
          DWC_Http_GetBase64DecodedResult(DWCauth->http, "datetime", DWCauth->result.datetime, 0xf);
          DWC_Http_GetRawResult(DWCauth->http, "Set-Cookie", DWCauth->result.cookie, 0x41);
          (DWCauth->result).cookie[0x2b] = '\0';
          if (lVar3 == 0x28) {
            DWCauthingamesncheckresult = DWCAUTH_E_MEMERR;
          }
          else {
            DWCauthingamesncheckresult = DWCAUTH_E_BMINITERR;
          }
        }
        uVar2 = 0;
      } else {
        DWC_Printf(DWC_REPORTFLAG_AUTH, "\tAuth : NAS RESPONSE CODE INVALID\n");
        DWCauth->result.returncode = 20101;
        uVar2 = 0xc;
      }
    }
  } else {
    DWC_Printf(DWC_REPORTFLAG_AUTH, "\tDWC_Auth : NON 200 HTTP RESPONSE CODE %d\n",iVar1);
    DWCauth->result.returncode = iVar1 + 23000;
    uVar2 = 0x12;
  }
  return uVar2;
}

DWCAuthError DWCi_Auth_Prepare_FirstPost(DWCHttp *http, const char *gsbrcd, DWCAuthParam * param, BOOL parse)
{
  size_t sVar1;
  int iVar2;
  undefined4 uVar3;
  char auStack_a8 [148];
  
  if (gsbrcd == NULL) {
    OSi_Panic("dwc_auth.c", 0x2b3, "Pointer must not be NULL (gsbrcd)");
  }
  if (DWC_auth_wifiinfo.attestedUserId == 0) {
    DWC_Printf(DWC_REPORTFLAG_AUTH, "Adding acctcreate command\n");
    sVar1 = strlen("acctcreate");
    iVar2 = DWC_Http_Add_PostBase64Item(http, "action", "acctcreate", sVar1);
    if (iVar2 != 0) {
      return 8;
    }
  } else {
    DWC_Printf(DWC_REPORTFLAG_AUTH, "Adding login command to HTTP POST packet\n");
    sVar1 = strlen("login");
    iVar2 = DWC_Http_Add_PostBase64Item(http, "action", "login", sVar1);
    if (iVar2 != 0) {
      return 8;
    }
    sVar1 = strlen(gsbrcd);
    iVar2 = DWC_Http_Add_PostBase64Item(http, "gsbrcd", gsbrcd, sVar1);
    if (iVar2 != 0) {
      return 8;
    }
  }
  iVar2 = DWC_Auth_GetCalInfoFromWiFiInfo(auStack_a8, &DWC_auth_wifiinfo);
  if (iVar2 == 0) {
    uVar3 = 5;
  } else {
    iVar2 = DWC_Auth_SetCalInfoToHttp(http,auStack_a8, DWCishotspot);
    if (iVar2 == 0) {
      uVar3 = 8;
    } else {
      if (DWCiswfcutil == 1) {
        sVar1 = strlen("Y");
        iVar2 = DWC_Http_Add_PostBase64Item(http, "iswfc", "Y", sVar1);
        if (iVar2 != 0) {
          return 8;
        }
      }
      sVar1 = wcslen((wchar_t *)param->ingamesn);
      if (sVar1 != 0) {
        sVar1 = wcslen((wchar_t*)param->ingamesn);
        iVar2 = DWC_Http_Add_PostBase64Item(http, "ingamesn", (char*)param->ingamesn, sVar1 << 1);
        if (iVar2 != 0) {
          return 8;
        }
      }
      uVar3 = 0;
    }
  }
  return uVar3;
}

void DWCi_Auth_SetError(DWCAuthError error)
{
	OS_LockMutex(&DWCauth->mutex);
	DWCauth->error = error;
	OS_UnlockMutex(&DWCauth->mutex);
}