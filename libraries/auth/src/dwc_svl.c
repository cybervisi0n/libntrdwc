#include <nitro.h>
#include <nitroWiFi.h>
#include "auth/dwc_auth.h"
#include "auth/dwc_http.h"
#include "auth/dwc_svl.h"

#include <errno.h>

#include "decomp/decomp_defs.h"

typedef struct {
  DWCAuthAlloc alloc;
  DWCAuthFree free;
  DWCHttp http;
  DWCSvlResult * result;
} DWCSvlIntWorkarea;

static DWCSvlIntWorkarea * intwork;
extern DWCHttpParam DWCauthhttpparam;

BOOL DWC_Svl_Init(DWCAuthAlloc alloc, DWCAuthFree free)
{
  BOOL uVar1;
  
  if (intwork == NULL) {
    intwork = (DWCSvlIntWorkarea *)(*alloc)((int)"ALLOC intwork", sizeof(DWCSvlIntWorkarea));
    if (intwork == NULL) {
      //DWCi_SetError(9,0xffffa1dc);
      uVar1 = 0;
    } else {
      MI_CpuClear8(intwork, sizeof(DWCSvlIntWorkarea));
      intwork->alloc = alloc;
      intwork->free = free;
      uVar1 = 1;
    }
  } else {
    //DWCi_SetError(9,0xffffa1dc);
    uVar1 = 0;
  }
  return uVar1;
}


void DWC_Svl_Cleanup(void)
{
  if (intwork != NULL) {
    DWC_Http_Destroy(&intwork->http);
    (*intwork->free)((int)"FREE intwork",intwork,0);
    intwork = NULL;
  }
  return;
}


BOOL DWC_Svl_GetTokenAsync(char *svc, DWCSvlResult *result)
{
  int iVar1;
  char *local_c0;
  undefined4 local_bc;
  undefined4 local_b8;
  //_func_void_ptr_unsigned_long_long *local_b4;
  //_func_void_unsigned_long_void_ptr_long *local_b0;
  uint local_ac;
  undefined4 local_a8;
  u8 auStack_a4 [152];
  
  if (intwork == NULL) {
    OSi_Panic("dwc_svl.c", 0x54, "Failed assertion intwork");
  }
  if (svc == NULL) {
    OSi_Panic("dwc_svl.c", 0x55, "Failed assertion svc");
  }
  if (result == NULL) {
    OSi_Panic("dwc_svl.c", 0x56, "Failed assertion result");
  }
  intwork->result = result;
  MI_CpuClear8(result, sizeof(DWCSvlResult));
  if (((intwork->http).thread.id != 0) &&
     (iVar1 = OS_IsThreadTerminated(&(intwork->http).thread), iVar1 == 0)) {
    OSi_Panic("dwc_svl.c", 0x5f, "You can't request SVL token while SVL module is working.\n");
  }
  //local_c0 = DWCauthhttpparam;
  local_bc = 0;
  local_b8 = 0x1000;
  iVar1 = strcmp(DWCauthhttpparam.url, "https://nas.nintendowifi.net/ac");
  local_ac = (uint)(iVar1 != 0);
  local_a8 = 20000;
  iVar1 = DWC_Http_Create(&intwork->http,&DWCauthhttpparam);
  if (iVar1 == 0) {
    /*iVar1 = */DWC_Auth_GetCalInfoFromNVRAM(auStack_a4);
    if (/*(iVar1 == 0) || */(iVar1 = DWC_Auth_SetCalInfoToHttp(&intwork->http,auStack_a4,0), iVar1 == 0)
       ) {
      DWC_Http_Destroy(&intwork->http);
      //DWCi_SetError(9,0xffffa1dc);
      iVar1 = 0;
    } else {
      iVar1 = DWC_Http_Add_PostBase64Item(&intwork->http, "action", "SVCLOC",6);
      if ((iVar1 == 0) &&
         (iVar1 = DWC_Http_Add_PostBase64Item(&intwork->http, "svc", svc, 4), iVar1 == 0)) {
        iVar1 = DWC_Http_FinishHeader(&intwork->http);
        if (iVar1 == 0) {
          OS_GetCurrentThread();
          //iVar1 = OS_GetThreadPriority();
          //TODO: Get the proper thread priority.
          iVar1 = 0x69;
          DWC_Http_StartThread(&intwork->http, iVar1 + -1);
          if ((intwork->http).thread.id == 0) {
            DWC_Http_Destroy(&intwork->http);
            //DWCi_SetError(9,0xffffa1dc);
            iVar1 = 0;
          }
          else {
            iVar1 = 1;
          }
        } else {
          DWC_Http_Destroy(&intwork->http);
          //DWCi_SetError(9,0xffffa1dc);
          iVar1 = 0;
        }
      } else {
        DWC_Http_Destroy(&intwork->http);
        //DWCi_SetError(9,0xffffa1dc);
        iVar1 = 0;
      }
    }
  } else {
    //DWCi_SetError(9,0xffffa1dc);
    iVar1 = 0;
  }
  return iVar1;
}


DWCSvlState	DWC_Svl_Process(void)
{
  undefined4 uVar1;
  int iVar2;
  DWCHttpError httpError;
  
  if (intwork == (DWCSvlIntWorkarea *)0x0) {
    uVar1 = 0;
  } else if ((intwork->http).thread.id == 0) {
    uVar1 = 1;
  } else {
    iVar2 = OS_IsThreadTerminated(&(intwork->http).thread);
    if (iVar2 == 1) {
      httpError = (intwork->http).error;
      if (httpError == DWCHTTP_E_ABORT) {
        DWC_Http_Destroy(&intwork->http);
        uVar1 = 5;
      }
      else if (httpError == DWCHTTP_E_FINISH) {
        iVar2 = DWCi_Svl_ParseHttp();
        if (iVar2 == 0) {
          DWC_Http_Destroy(&intwork->http);
          uVar1 = 4;
        }
        else {
          DWC_Http_Destroy(&intwork->http);
          uVar1 = 3;
        }
      }
      else {
        DWC_Http_Destroy(&intwork->http);
        //DWCi_SetError(0x11,0xffffa1db);
        uVar1 = 4;
      }
    } else {
      uVar1 = 2;
    }
  }
  return uVar1;
}


void DWC_Svl_Abort(void)
{
  int iVar1;
  
  if (((intwork != NULL) && ((intwork->http).thread.id != 0)) &&
     (iVar1 = OS_IsThreadTerminated(&(intwork->http).thread), iVar1 == 0)) {
    DWC_Http_Abort(&intwork->http);
  }
  return;
}


int DWCi_Svl_ParseHttp(void)
{
  int iVar1;
  undefined4 uVar2;
  char *__nptr;
  undefined4 in_r3;
  DWCHttp *myHttp;
  char local_18 [4];
  undefined4 uStack_14;
  char * httpResult;
  
  myHttp = &intwork->http;
  uStack_14 = in_r3;
  iVar1 = DWC_Http_ParseResult(myHttp,0);
  if (iVar1 == 0) {
    //DWCi_SetError(0x10,0xffff9df3);
    uVar2 = 0;
  } else {
    httpResult = DWC_Http_GetResult(myHttp, "Date");
    if (httpResult != 0) {
      //TODO!! File not implemented yet
      //DWC_Auth_SetNasTimeDiff(iVar1);
    }
    __nptr = (char *)DWC_Http_GetResult(myHttp, "httpresult");
    if (__nptr == (char *)0x0) {
      //DWCi_SetError(0x10,0xffff9df3);
      uVar2 = 0;
    } else {
      iVar1 = atoi(__nptr);
      if (errno == 0x22) {
        //DWCi_SetError(0x10,0xffff9df3);
        uVar2 = 0;
      } else if (iVar1 == 200) {
        iVar1 = DWC_Http_GetBase64DecodedResult(myHttp, "returncd", local_18, 4);
        if (iVar1 < 1) {
          //DWCi_SetError(0x10,0xffff9df3);
          uVar2 = 0;
        } else {
          iVar1 = atoi(local_18);
          if (errno == 0x22) {
            //DWCi_SetError(0x10,0xffff9df3);
            uVar2 = 0;
          } else if (iVar1 < 100) {
            DWC_Http_GetBase64DecodedResult(myHttp, "svchost", intwork->result->svlhost, 0x40);
            DWC_Http_GetBase64DecodedResult(myHttp, "servicetoken", intwork->result->svltoken, 300);
            DWC_Http_GetBase64DecodedResult(myHttp, "statusdata", local_18, 1);
            if (local_18[0] == 'Y') {
              intwork->result->status = 1;
            } else {
              intwork->result->status = 0;
            }
            uVar2 = 1;
          } else {
            //DWCi_SetError(0x10,-24000 - iVar1);
            uVar2 = 0;
          }
        }
      } else {
        //DWCi_SetError(0x10,-25000 - iVar1);
        uVar2 = 0;
      }
    }
  }
  return uVar2;
}


