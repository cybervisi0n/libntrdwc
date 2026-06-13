#include <nitro.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"
#include "ac_private.h"

static int GetIrregularError(void);
static int GetNotFoundAP    ( ACHOLD *hd );
static int GetNotFoundInet  ( ACHOLD *hd );
static int GetProgramaError(int error);

u8 DWCi_AC_Error(void)
{
  u8 uVar1;
  tagACHOLD *myAcHold;
  int iVar2;
  
  myAcHold = (tagACHOLD *)DWCi_AC_GetMemPtr(1);
  iVar2 = DWCi_AC_CloseNetwork(&myAcHold->phaseError);
  if (iVar2 == 1) {
    uVar1 = '\x12';
  }
  else {
    uVar1 = '\x11';
  }
  return uVar1;
}

int DWCi_AC_GetResult(void)
{
  tagACHOLD *myAcHold;
  int iVar1;
  
  myAcHold = (tagACHOLD *)DWCi_AC_GetMemPtr(1);
  iVar1 = DWCi_AC_GetError();
  if (iVar1 < 4) {
    iVar1 = GetProgramaError(iVar1);
  }
  else if (iVar1 < 5) {
    iVar1 = GetIrregularError();
  }
  else if (iVar1 == 5) {
    iVar1 = GetNotFoundAP(myAcHold);
  }
  else {
    iVar1 = GetNotFoundInet(myAcHold);
  }
  return iVar1;
}

static int GetProgramaError(int error)
{
  int iVar1;
  
  switch(error) {
  case 0:
    iVar1 = -10;
    break;
  case 1:
    iVar1 = -9;
    break;
  case 2:
    iVar1 = -8;
    break;
  case 3:
    iVar1 = -7;
    break;
  default:
    iVar1 = 0;
  }
  return iVar1;
}

static int GetIrregularError(void)
{
  return 0xfffffffa;
}

static int GetNotFoundAP    ( tagACHOLD *hd )
{
  int iVar1;
  
  if (hd->findAP == '\0') {
    iVar1 = -0xc3b3;
  }
  else {
    iVar1 = -0xc79b;
  }
  return iVar1;
}

static int GetNotFoundInet(tagACHOLD * hd)
{
  byte bVar1;
  int iVar2;
  uint uVar3;
  
  if (hd->overrideType == '\0') {
    bVar1 = hd->endType;
  }
  else {
    bVar1 = hd->overrideType;
  }
  uVar3 = (uint)bVar1;
  if (hd->endSequence < 10) {
    if (hd->endState == '\x03') {
      iVar2 = -0xc79c - uVar3;
    }
    else if (hd->endState == '\x04') {
      iVar2 = -0xc800 - uVar3;
    }
    else {
      iVar2 = -0xc864 - uVar3;
    }
  }
  else if (hd->endSequence < 0xd) {
    iVar2 = -52000 - uVar3;
  }
  else if (hd->errorTest == 0) {
    iVar2 = -50000 - uVar3;
  }
  else if (hd->errorTest == -1) {
    iVar2 = -0xcb84 - uVar3;
  }
  else if (hd->errorTest == -2) {
    iVar2 = -0xcbe8 - uVar3;
  }
  else if (hd->errorTest == -3) {
    iVar2 = -0xcc4c - uVar3;
  }
  else if (hd->errorTest == -4) {
    iVar2 = -53000 - uVar3;
  }
  else if (hd->errorTest == -5) {
    iVar2 = -0xcf6c - uVar3;
  }
  else if (hd->errorTest == -6) {
    iVar2 = -0xcfd0 - uVar3;
  }
  else {
    iVar2 = hd->errorTest;
  }
  return iVar2;
}