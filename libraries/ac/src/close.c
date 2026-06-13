#include <nitro.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"

static undefined4 DisConnectAP(void);
static undefined4 CloseSocket(void);

undefined4 DWCi_AC_CloseNetwork(byte *param_1)
{
  int iVar1;
  
  if (*param_1 < 0xb) {
    iVar1 = DisConnectAP();
    if (iVar1 == 1) {
      *param_1 = 0;
      return 1;
    }
    if (iVar1 == -1) {
      *param_1 = 0x12;
      return 1;
    }
  }
  else if (*param_1 == 0xe) {
    //DWC_Netcheck_Abort();
    //DWC_Netcheck_Destroy();
    *param_1 = 0xc;
  }
  else if ((*param_1 < 0x12) && (iVar1 = CloseSocket(), iVar1 == 1)) {
    *param_1 = 10;
  }
  return 0;
}

static undefined4 DisConnectAP(void)
{
  undefined4 uVar1;
  
  uVar1 = WCM_GetPhase();
  switch(uVar1) {
  case 0:
    return 1;
  case 1:
    WCM_Finish();
    break;
  case 2:
    break;
  case 3:
    WCM_CleanupAsync();
    break;
  case 4:
    break;
  case 5:
    break;
  case 6:
    WCM_EndSearchAsync();
    break;
  case 7:
    break;
  case 8:
    break;
  case 9:
    WCM_DisconnectAsync();
    break;
  case 10:
    break;
  case 0xb:
    //DWCi_AC_SetError(0);
    return 0xffffffff;
  case 0xc:
    WCM_TerminateAsync();
  }
  return 0;
}

static undefined4 CloseSocket(void)
{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = SOCL_CalmDown();
  if (iVar1 == 0) {
    iVar1 = SOC_Cleanup();
    if ((iVar1 == 0) || (iVar1 == -0x27)) {
      //DWC_Printf(0x2000000,_13062,iVar1);
      uVar2 = 1;
    }
    else {
      uVar2 = 0;
    }
  }
  else {
    //DWC_Printf(0x2000000,_13061);
    uVar2 = 0;
  }
  return uVar2;
}


