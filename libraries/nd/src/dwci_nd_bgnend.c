#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

DWCNdError DWCi_Ndi_error;
BOOL DWCi_Ndi_isOpened;
DWCNdAlloc DWCi_Ndi_alloc;
DWCNdFree DWCi_Ndi_free;
char * DWCi_Ndi_dlattr[3];
char * DWCi_Ndi_authtoken;
char * DWCi_Ndi_serverURL;
int DWCi_Ndi_nhttpid;
DWCNdCallback DWCi_Ndi_dlcallback;
int DWCi_Ndi_isExistApinfo;
char DWCi_Ndi_password[25];
char DWCi_Ndi_dwcgamecd[9];
char DWCi_Ndi_userAgent[33];
char DWCi_Ndi_macaddr[17];
char DWCi_Ndi_strApinfo[21];

static DWCNdCleanupCallback cleanupCallback;

static void nhttpCleanupCallback(void);
static void * strAlloc(char * param_1);

static void nhttpCleanupCallback(void)
{
  #ifndef SDK_PORT

  DWCi_Ndi_freeAttr();
  #endif
  DWCi_Ndi_exitLockGlobal();
  DWCi_Ndi_exitHttpCallback();
  (*DWCi_Ndi_free)(DWCi_Ndi_authtoken);
  (*DWCi_Ndi_free)(DWCi_Ndi_serverURL);
  (*cleanupCallback)();
  return;
}

static void * strAlloc(char * param_1)
{
  int iVar1;
  void *pvVar2;
  
  iVar1 = DWCi_Ndi_strlen(param_1);
  pvVar2 = (*DWCi_Ndi_alloc)(iVar1 + 1,4);
  if (pvVar2 != (void *)0x0) {
    DWCi_Ndi_memcpy(pvVar2,param_1,iVar1);
    *(u8 *)((u8*)pvVar2 + iVar1) = 0;
  }
  return pvVar2;
}

void DWCi_NdCleanupAsync(DWCNdCleanupCallback param_1)
{
  if (DWCi_Ndi_isOpened == 0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0xde, "Failed assertion DWCi_Ndi_isOpened");
  }
  if (param_1 == NULL) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0xdf, "Failed assertion callback");
  }
  DWCi_Ndi_isOpened = 0;
  cleanupCallback = param_1;
  NHTTP_CleanupAsync(nhttpCleanupCallback);
  return;
}

DWCNdError DWCi_NdGetError(void)
{
  if (DWCi_Ndi_isOpened == 0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0xf1, "Failed assertion DWCi_Ndi_isOpened");
  }
  return DWCi_Ndi_error;
}

BOOL DWCi_NdStartup( DWCNdAlloc alloc, 
                     DWCNdFree free, 
                     char* url, 
                     char* token, 
                     char* gamecd, 
                     char* passwd, 
                     DWCNdCallback callback )
{
  int iVar1;
  
  if (DWCi_Ndi_isOpened != 0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0x88, "Failed assertion !DWCi_Ndi_isOpened");
  }
  if (alloc == (void *)0x0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0x89, "Failed assertion alloc");
  }
  if (free == (void *)0x0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0x8a, "Failed assertion free");
  }
  if (url == (char *)0x0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0x8b, "Failed assertion url");
  }
  if (token == (char *)0x0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0x8c, "Failed assertion token");
  }
  if (gamecd == (char *)0x0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0x8d, "Failed assertion gamecd");
  }
  if (passwd == (char *)0x0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0x8e, "Failed assertion passwd");
  }
  if (callback == (void *)0x0) {
    OSi_Panic("DWCi_Nd_bgnend.c", 0x8f, "Failed assertion callback");
  }
  DWCi_Ndi_dlcallback = callback;
  DWCi_Ndi_error = DWC_ND_CBREASON_INITIALIZE;
  DWCi_Ndi_dlattr[0] = NULL;
  DWCi_Ndi_dlattr[1] = NULL;
  DWCi_Ndi_dlattr[2] = NULL;
  DWCi_Ndi_isExistApinfo = 0;
  DWCi_Ndi_nhttpid = -1;
  DWCi_Ndi_authtoken = NULL;
  DWCi_Ndi_serverURL = NULL;
  DWCi_Ndi_alloc = alloc;
  DWCi_Ndi_free = free;
  DWCi_Ndi_getMacAddress(DWCi_Ndi_macaddr);
  DWCi_Ndi_getUserAgent(DWCi_Ndi_userAgent);
  DWCi_Ndi_serverURL = (char *)strAlloc(url);
  if ((DWCi_Ndi_serverURL != NULL) &&
     (DWCi_Ndi_authtoken = (char *)strAlloc(token), DWCi_Ndi_authtoken != (char *)0x0)) {
    iVar1 = DWCi_Ndi_encodeBASE64(DWCi_Ndi_dwcgamecd,gamecd,4);
    DWCi_Ndi_dwcgamecd[iVar1] = '\0';
    iVar1 = DWCi_Ndi_encodeBASE64(DWCi_Ndi_password,passwd,0x10);
    DWCi_Ndi_password[iVar1] = '\0';
    iVar1 = DWCi_Ndi_initHttpCallback();
    if ((iVar1 != 0) && (iVar1 = DWCi_Ndi_initLockGlobal(), iVar1 != 0)) {
      iVar1 = NHTTP_Startup(alloc,free,10);
      if (iVar1 != 0) {

        DWCi_Ndi_isOpened = 1;
        return 1;
      }
      DWCi_Ndi_exitLockGlobal();
    }
    (*DWCi_Ndi_free)(DWCi_Ndi_authtoken);
    (*DWCi_Ndi_free)(DWCi_Ndi_serverURL);
    DWCi_Ndi_error = DWC_ND_ERROR_FATAL;
    return 0;
  }
  if (DWCi_Ndi_authtoken != (char *)0x0) {
    (*DWCi_Ndi_free)(DWCi_Ndi_authtoken);
  }
  if (DWCi_Ndi_serverURL != (char *)0x0) {
    (*DWCi_Ndi_free)(DWCi_Ndi_serverURL);
  }
  DWCi_Ndi_error = DWC_ND_CBREASON_GETFILELISTNUM;
  return 0;
}