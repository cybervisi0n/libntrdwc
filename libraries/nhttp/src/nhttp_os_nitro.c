#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"


void * g_CommThreadStack;
OSThread g_CommThread;
OSMessageQueue g_CommThreadMessageQueue;
OSMutex g_MutexIdList;

extern int NHTTPi_isThreadEnd;

#ifndef SDK_BUILD_ARM
OSMessage g_CommThreadMessageArray[120];
#endif

BOOL NHTTPi_createCommThread(u32 prio, void * stack)
{
  int iVar1;
  
  #ifdef SDK_BUILD_ARM
  iVar1 = OS_IsThreadAvailable();
  #endif
  iVar1 = TRUE;
  if (iVar1 != 0) {
    #ifdef SDK_BUILD_ARM
    OS_InitMessageQueue(&g_CommThreadMessageQueue,(void*)0x10000,1);
    #else
    OS_InitMessageQueue(&g_CommThreadMessageQueue,g_CommThreadMessageArray,1);
    #endif
    OS_CreateThread(&g_CommThread, NHTTPi_CommThreadProc,(void *)0x0,(void *)(stack + 0x2000),
                    0x2000,prio);
    OS_WakeupThreadDirect(&g_CommThread);
  }
  return iVar1 != 0;
}


void NHTTPi_DBGOUT_STR_(char *aString)
{
  OS_PutString(aString);
  return;
}


void NHTTPi_destroyCommThread(void)
{
  NHTTPi_isThreadEnd = 1;
  NHTTPi_kickCommThread();
  OS_JoinThread(&g_CommThread);
  return;
}


void NHTTPi_exitLockReqList(void)
{
  return;
}


void NHTTPi_idleCommThread(void)
{
  undefined4 in_r3;
  undefined4 auStack_8 [2];
  
  auStack_8[0] = in_r3;
  OS_ReceiveMessage(&g_CommThreadMessageQueue,(void*)auStack_8,1);
  return;
}


BOOL NHTTPi_initLockReqList(void)
{
  OS_InitMutex(&g_MutexIdList);
  return 1;
}


void NHTTPi_kickCommThread(void)
{ 
  OS_SendMessage(&g_CommThreadMessageQueue,NULL,0);
  return;
}


void NHTTPi_lockReqList(void)
{
  OS_LockMutex(&g_MutexIdList);
  return;
}


void NHTTPi_unlockReqList(void)
{
  OS_UnlockMutex(&g_MutexIdList);
  return;
}
