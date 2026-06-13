#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"

extern DWCNdError DWCi_Ndi_error;
extern BOOL DWCi_Ndi_isOpened;
extern DWCNdAlloc DWCi_Ndi_alloc;
extern DWCNdFree DWCi_Ndi_free;
extern char * DWCi_Ndi_dlattr[3];

static BOOL setAttr(int param_1, char *param_2)
{
  int iVar1;
  int iVar2;
  char * uVar3;
  
  if (*param_2 != '\0') {
    iVar1 = DWCi_Ndi_strlen(param_2);
    if (10 < iVar1) {
      DWCi_Ndi_error = 7;
      return 0;
    }
    iVar1 = DWCi_Ndi_strlen(param_2);
    if (10 < iVar1) {
      DWCi_Ndi_error = 7;
      return 0;
    }
    iVar2 = DWCi_Ndi_getEncodeLenBASE64(iVar1);
    uVar3 = (*DWCi_Ndi_alloc)(iVar2 + 1,4);
    (DWCi_Ndi_dlattr)[param_1] = uVar3;
    if ((DWCi_Ndi_dlattr)[param_1] == NULL) {
      DWCi_Ndi_error = 1;
      return 0;
    }
    iVar1 = DWCi_Ndi_encodeBASE64((DWCi_Ndi_dlattr)[param_1],param_2,iVar1);
    *(u8 *)((&DWCi_Ndi_dlattr)[param_1] + iVar1) = 0;
  }
#ifdef SDK_PORT
  else 
  {

    (DWCi_Ndi_dlattr)[param_1] = param_2;
  }
#endif
  return 1;
}

void DWCi_Ndi_freeAttr(void)
{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < 3; iVar1 = iVar1 + 1) {
    if ((DWCi_Ndi_dlattr)[iVar1] != NULL) {
      (*DWCi_Ndi_free)((DWCi_Ndi_dlattr)[iVar1]);
      (DWCi_Ndi_dlattr)[iVar1] = NULL;
    }
  }
  return;
}

 
BOOL DWCi_NdSetAttr( char* attr1, char* attr2, char* attr3 )
{
  int iVar1;
  BOOL ret;
  
  if (DWCi_Ndi_isOpened == 0) {
    OSi_Panic("DWCi_Nd_attr.c", 0x6c, "Failed assertion DWCi_Ndi_isOpened");
  }
  if (attr1 == NULL) {
    OSi_Panic("DWCi_Nd_attr.c", 0x6d, "Failed assertion attr1");
  }
  if (attr2 == NULL) {
    OSi_Panic("DWCi_Nd_attr.c", 0x6e, "Failed assertion attr2");
  }
  if (attr3 == NULL) {
    OSi_Panic("DWCi_Nd_attr.c", 0x6f, "Failed assertion attr3");
  }
  DWCi_Ndi_lockGlobal();
  DWCi_Ndi_freeAttr();
  iVar1 = setAttr(0,attr1);
  if (((iVar1 == 0) || (iVar1 = setAttr(1,attr2), iVar1 == 0)) ||
     (iVar1 = setAttr(2,attr3), iVar1 == 0)) {
    DWCi_Ndi_freeAttr();
    DWCi_Ndi_unlockGlobal();
    ret = FALSE;
  } else {
    DWCi_Ndi_unlockGlobal();
    ret = TRUE;
  }
  return ret;
}