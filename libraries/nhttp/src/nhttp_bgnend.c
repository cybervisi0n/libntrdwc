#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

NHTTPAlloc NHTTPi_alloc;
NHTTPFree NHTTPi_free;
NHTTPError NHTTPi_error;
int NHTTPi_idRequest;
int NHTTPi_isOpened;
extern void * g_CommThreadStack;
OSThread g_CleanupThread;
u8 g_CleanupThreadStack[0x800];
int NHTTPi_isThreadEnd;

tagNHTTPi_REQLIST * NHTTPi_reqQueue;
tagNHTTPi_REQLIST * NHTTPi_reqCurrent;


void NHTTP_CleanupAsync( NHTTPCleanupCallback callback )
{
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_bgnend.c" ,0x86, "Failed assertion NHTTPi_isOpened");
  }
  if (callback == 0) {
    OSi_Panic("NHTTP_bgnend.c" ,0x87, "Failed assertion callback");
  }
  OS_CreateThread(&g_CleanupThread, NHTTPi_CleanupThreadProc, callback, (u8*)g_CleanupThreadStack + 0x800,0x800,0x10); //TODO: passing string literal as stack for the thread seems wrong
  OS_WakeupThreadDirect(&g_CleanupThread);
  return;
}


void NHTTPi_CleanupThreadProc(void * callback)
{
  NHTTPCleanupCallback myCallback = (NHTTPCleanupCallback)callback;
  if (myCallback == NULL) {
    OSi_Panic("NHTTP_bgnend.c", 0xa8, "Failed assertion callback");
  }
  NHTTPi_cancelAllRequests();
  NHTTPi_destroyCommThread();
  (*NHTTPi_free)(g_CommThreadStack);
  NHTTPi_exitLockReqList();
  NHTTPi_isOpened = 0;
  (*myCallback)();
  return;
}


NHTTPError NHTTP_GetError(void)
{
  if (NHTTPi_isOpened == 0) {
    OSi_Panic("NHTTP_bgnend.c", 0x99, "Failed assertion NHTTPi_isOpened");
  }

  return NHTTPi_error;
}


BOOL NHTTP_Startup( NHTTPAlloc alloc, NHTTPFree free, u32 threadprio )
{
  int iVar1;
  undefined4 uVar2;
  
  if (NHTTPi_isOpened != 0) {
    OSi_Panic("NHTTP_bgnend.c", 0x53, "Failed assertion !NHTTPi_isOpened");
  }
  if (alloc == NULL) {
    OSi_Panic("NHTTP_bgnend.c", 0x54, "Failed assertion alloc");
  }
  if (free == NULL) {
    OSi_Panic("NHTTP_bgnend.c", 0x55, "Failed assertion free");
  }
  if (0x1f < threadprio) {
    OSi_Panic("NHTTP_bgnend.c", 0x56, "Failed assertion (threadprio >= 0) && (threadprio <= 31)");
  }
  NHTTPi_error = NHTTP_ERROR_NONE;
  NHTTPi_idRequest = 0;
  NHTTPi_reqQueue = NULL;
  NHTTPi_reqCurrent = NULL;
  NHTTPi_isThreadEnd = 0;
  NHTTPi_alloc = alloc;
  NHTTPi_free = free;
  iVar1 = NHTTPi_initLockReqList();
  if (iVar1 == 0) {
    NHTTPi_error = NHTTP_ERROR_NITROSDK;
    uVar2 = 0;
  } else {
    g_CommThreadStack = (void *)(*NHTTPi_alloc)(0x2000, 8);
    if (g_CommThreadStack == NULL) {
      NHTTPi_error = NHTTP_ERROR_ALLOC;
      NHTTPi_exitLockReqList();
      uVar2 = 0;
    } else {
      iVar1 = NHTTPi_createCommThread(threadprio, g_CommThreadStack);
      if (iVar1 == 0) {
        NHTTPi_error = NHTTP_ERROR_NITROSDK;
        (*NHTTPi_free)(g_CommThreadStack);
        NHTTPi_exitLockReqList();
        uVar2 = 0;
      }
      else {
        NHTTPi_isOpened = 1;
        uVar2 = 1;
      }
    }
  }
  return uVar2;
}