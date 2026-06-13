#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"

extern DWCNdAlloc DWCi_Ndi_alloc;
extern DWCNdFree DWCi_Ndi_free;

int g_rightHTTPComm;
static OSMutex g_MutexAttr;
static OSMutex g_MutexHttp;

void DWCi_Ndi_destroyThreadEvent(void * param_1)
{
  (*DWCi_Ndi_free)(param_1);
  return;
}


void DWCi_Ndi_exitHttpCallback(void)
{
  return;
}


void DWCi_Ndi_exitLockGlobal(void)
{
  return;
}


void DWCi_Ndi_getMacAddress(char *aMacAddr)
{
  int iVar1;
  u8 local_30 [24];
  
  OS_GetMacAddress(local_30);
  for (iVar1 = 0; iVar1 < 6; iVar1 = iVar1 + 1) {
    //DWCi_Ndi_hexToStrFigure(local_30 + iVar1 * 2 + 6,local_30[iVar1],2);
  }
  DWCi_Ndi_encodeBASE64(aMacAddr, local_30 + 6,0xc);
  return;
}


BOOL DWCi_Ndi_initHttpCallback(void)
{
  OS_InitMutex(&g_MutexHttp);
  g_rightHTTPComm = 0;
  return 1;
}


BOOL DWCi_Ndi_initLockGlobal(void)
{
  OS_InitMutex(&g_MutexAttr);
  return 1;
}


void DWCi_Ndi_kickThread(OSMessageQueue *param_1)
{
  OS_SendMessage(param_1,(void *)0x0,0);
  return;
}


void DWCi_Ndi_lockGlobal(void)
{
  OS_LockMutex(&g_MutexAttr);
  return;
}


BOOL DWCi_Ndi_prepareHttpCallback(void)
{
  BOOL bVar1;
  
  OS_LockMutex(&g_MutexHttp);
  bVar1 = g_rightHTTPComm == 0;
  if (bVar1) {
    g_rightHTTPComm = 1;
  }
  OS_UnlockMutex(&g_MutexHttp);
  return bVar1;
}


void DWCi_Ndi_unlockGlobal(void)
{
  OS_UnlockMutex(&g_MutexAttr);
  return;
}


void DWCi_Ndi_unprepareHttpCallback(void)
{
  OS_LockMutex(&g_MutexHttp);
  g_rightHTTPComm = 0;
  OS_UnlockMutex(&g_MutexHttp);
  return;
}