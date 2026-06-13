#include <nitro.h>
#include <nitro/os.h>
#include <nitro/rtc.h>

#include <nitroWiFi.h>
#include <string.h>
#include "auth/dwc_auth.h"
#include "auth/dwc_http.h"
#include "auth/dwc_netcheck.h"
#include "decomp/decomp_defs.h"

#ifdef SDK_BUILD_LINUX
#include <wchar.h>
#endif


void DWC_Auth_GetCalInfoFromNVRAM(char * aBuf)
{
  DWCWiFiInfo myWifiInfo;
  char auStack_28 [24];
  undefined4 local_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  //local_10 = param_1;
  //uStack_c = param_2;
  //uStack_8 = param_3;
  //uStack_4 = param_4;
  //DWCi_BM_GetWiFiInfo(auStack_28);
  DWCi_BM_GetWiFiInfo(&myWifiInfo);
  DWC_Auth_GetCalInfoFromWiFiInfo(aBuf, &myWifiInfo);
  return;
}


undefined4 DWC_Auth_GetCalInfoFromWiFiInfo(char * param_1, DWCWiFiInfo *aWifiInfo)
{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  u16 essid [2];
  u8 macAddr [6];
  //undefined4 local_94;
  RTCTime theTime;
  undefined4 local_90;
  undefined4 local_8c;
  //undefined4 local_88;
  RTCDate theDate;
  undefined4 local_84;
  undefined4 local_80;
  //byte local_78 [2];
  OSOwnerInfo myOwnerInfo;
  //undefined local_76;
  //undefined local_75;
  //undefined auStack_74 [80];


  MI_CpuClear8(param_1,0x94);
  if (aWifiInfo->attestedUserId == 0) {
    iVar1 = aWifiInfo->notAttestedId;
    OS_SNPrintf(param_1, 0xe, "%013llu", aWifiInfo->notAttestedId, iVar1);
  } else {
    iVar1 = aWifiInfo->attestedUserId;
    OS_SNPrintf(param_1, 0xe, "%013llu", aWifiInfo->attestedUserId, iVar1);
  }
  OS_SNPrintf(param_1 + 0xe, 7, "%03u", aWifiInfo->pass, iVar1);
  //PCPORT_TODO
  if (/*DAT_027ffe0c == '\0'*/ FALSE) {
    OSi_Panic("dwc_cal.c", 0x48, "The GameCode in this ROM header is invalid. Check your ROM header template.\n");
  }
  //MI_CpuCopy8(&DAT_027ffe0c, param_1 + 0x15,4);
  //Copy Game Code
  MI_CpuCopy8((void *)(HW_ROM_HEADER_BUF+0xc), param_1 + 0x15,4);
  if (/*DAT_027ffe10 == '\0'*/ FALSE) {
    OSi_Panic("dwc_cal.c", 0x4d, "The MakerCode in this ROM header is invalid. Check your .rsf file.\n");
  }
  //MI_CpuCopy8(&DAT_027ffe10,param_1 + 0x1a,2);
  //Copy Maker Code
  MI_CpuCopy8((void *)(HW_ROM_HEADER_BUF+0x10), param_1 + 0x1a,2);
  param_1[29] = '0';
  OS_GetMacAddress(macAddr);
  for (iVar1 = 0; iVar1 < 6; iVar1 = iVar1 + 1) {
    OS_SPrintf(param_1 + 0x1f + iVar1 * 2, "%02x", macAddr[iVar1]);
  }
  OS_GetOwnerInfo(&myOwnerInfo);
  if (7 < myOwnerInfo.language) {
    myOwnerInfo.language = 1;
  }
  OS_SNPrintf(param_1 + 0x2c,3, "%02x", myOwnerInfo.language);
  //MI_CpuCopy8(auStack_74, param_1 + 0x7e, 0x14);
  MI_CpuCopy8(myOwnerInfo.nickName, param_1 + 0x7e, 0x14);
  //OS_SNPrintf(param_1 + 0x2f, 5, "%02x%02x", local_76, local_75);
  OS_SNPrintf(param_1 + 0x2f, 5, "%02x%02x", myOwnerInfo.birthday.day, myOwnerInfo.birthday.month);
  iVar1 = RTC_GetDateTime(&theDate,&theTime);
  if (iVar1 == 0) {
    OS_SNPrintf(param_1 + 0x34, 0xd, "%02d%02d%02d%02d%02d%02d", theDate.year, theDate.month, theDate.day, theTime.hour, theTime.minute, theTime.second);
    uVar2 = OS_DisableInterrupts();
    //iVar1 = WCM_GetApMacAddress();
    //DC_InvalidateRange(iVar1,6);
    if (iVar1 == 0) {
      OS_RestoreInterrupts(uVar2);
      //uVar2 = 0;
      uVar2 = 1; // TODO
    } else {
      for (iVar4 = 0; iVar4 < 6; iVar4 = iVar4 + 1) {
        OS_SPrintf(param_1 + 0x41 + iVar4 * 2, "%02x",*(u8 *)(iVar1 + iVar4));
      }
      uVar3 = DWC_AC_GetApType();
      if ((uVar3 == 0xff) || (99 < uVar3)) {
        uVar3 = 99;
      }
      OS_SNPrintf(param_1 + 0x6f, 0xe, "%02d:0000000-00", uVar3);
      //iVar1 = WCM_GetApEssid(essid);
      //DC_InvalidateRange(iVar1, 0x20);
      if (iVar1 == 0) {
        OS_RestoreInterrupts(uVar2);
        uVar2 = 0;
      } else {
        //MI_CpuCopy8(iVar1, param_1 + 0x4e,0x20);
        //DWC_AC_GetApSpotInfo(param_1 + 0x72);
        OS_RestoreInterrupts(uVar2);
        uVar2 = 1;
      }
    }
  } else {
    uVar2 = 0;
  }
  return uVar2;
}

undefined4 DWC_Auth_SetCalInfoToHttp(DWCHttp *aHttp,char *param_2,int param_3)
{
  size_t sVar1;
  int iVar2;
  undefined4 uVar3;
  char acStack_44 [36];
  
  OS_SNPrintf(acStack_44, 0x21, "%03d%03d", 2, 2);
  sVar1 = strlen(acStack_44);
  iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "sdkver", acStack_44, sVar1);
  if (iVar2 == 0) {
    sVar1 = strlen(param_2);
    iVar2 = DWC_Http_Add_PostBase64Item(aHttp,"userid", param_2, sVar1);
    if (iVar2 == 0) {
      sVar1 = strlen(param_2 + 0xe);
      iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "passwd", param_2 + 0xe, sVar1);
      if (iVar2 == 0) {
        sVar1 = strlen(param_2 + 0x41);
        // Add a dummy BSSID
        iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "bssid", "000000000001", strlen("000000000001"));
        if (iVar2 == 0) {
          sVar1 = strlen(param_2 + 0x6f);
          // Add a dummy APInfo
          iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "apinfo", "00:0000000-00", strlen("00:0000000-00"));
          if (iVar2 == 0) {
            sVar1 = strlen(param_2 + 0x15);
            iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "gamecd", param_2 + 0x15, sVar1);
            if (iVar2 == 0) {
              sVar1 = strlen(param_2 + 0x1a);
              iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "makercd", param_2 + 0x1a, sVar1);
              if (iVar2 == 0) {
                sVar1 = strlen(param_2 + 0x1d);
                iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "unitcd", param_2 + 0x1d, sVar1);
                if (iVar2 == 0) {
                  sVar1 = strlen(param_2 + 0x1f);
                  iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "macadr", param_2 + 0x1f, sVar1);
                  if (iVar2 == 0) {
                    sVar1 = strlen(param_2 + 0x2c);
                    iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "lang", param_2 + 0x2c, sVar1);
                    if (iVar2 == 0) {
                      sVar1 = strlen(param_2 + 0x2f);
                      iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "birth", param_2 + 0x2f, sVar1);
                      if (iVar2 == 0) {
                        sVar1 = strlen(param_2 + 0x34);
                        iVar2 = DWC_Http_Add_PostBase64Item(aHttp, "devtime", param_2 + 0x34, sVar1);
                        if (iVar2 == 0) {
                          sVar1 = wcslen((wchar_t *)(param_2 + 0x7e));
                          iVar2 = DWC_Http_Add_PostBase64Item
                                            (aHttp, "devname", param_2 + 0x7e, sVar1 << 1);
                          if (iVar2 == 0) {
                            if (param_3 == 1) {
                              sVar1 = strlen(param_2 + 0x4e);
                              iVar2 = DWC_Http_Add_PostBase64Item
                                                (aHttp, "ssid", param_2 + 0x4e, sVar1);
                              if (iVar2 != 0) {
                                return 0;
                              }
                            }
                            OS_SNPrintf(acStack_44, 0x21, "Nitro WiFi SDK/%d.%d", 2, 2);
                            iVar2 = DWC_Http_Add_HeaderItem(aHttp, "User-Agent", acStack_44);
                            if (iVar2 == 0) {
                              iVar2 = DWC_Http_Add_HeaderItem(aHttp, "HTTP_X_GAMECD", param_2 + 0x15);
                              if (iVar2 == 0) {
                                uVar3 = 1;
                              } else {
                                uVar3 = 0;
                              }
                            } else {
                              uVar3 = 0;
                            }
                          } else {
                            uVar3 = 0;
                          }
                        } else {
                          uVar3 = 0;
                        }
                      } else {
                        uVar3 = 0;
                      }
                    } else {
                      uVar3 = 0;
                    }
                  } else {
                    uVar3 = 0;
                  }
                } else {
                  uVar3 = 0;
                }
              } else {
                uVar3 = 0;
              }
            } else {
              uVar3 = 0;
            }
          } else {
            uVar3 = 0;
          }
        } else {
          uVar3 = 0;
        }
      } else {
        uVar3 = 0;
      }
    } else {
      uVar3 = 0;
    }
  } else {
    uVar3 = 0;
  }
  return uVar3;
}


void DWC_Auth_PrintCalInfo(char * param_1)
{
  DWC_Printf(DWC_REPORTFLAG_AUTH, "Printing current CAL DATA\n");
  DWC_Printf(DWC_REPORTFLAG_AUTH, "userid  = %s\n", param_1);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "passwd  = %s\n", param_1 + 0xe);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "gamecd  = %s\n", param_1 + 0x15);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "makercd = %s\n", param_1 + 0x1a);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "unitcd  = %s\n", param_1 + 0x1d);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "macadr  = %s\n", param_1 + 0x1f);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "lang    = %s\n", param_1 + 0x2c);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "devname = %s\n", param_1 + 0x7e);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "birth   = %s\n", param_1 + 0x2f);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "devtime = %s\n", param_1 + 0x34);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "bssid   = %s\n", param_1 + 0x41);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "apinfo  = %s\n", param_1 + 0x6f);
  DWC_Printf(DWC_REPORTFLAG_AUTH, "\n");
  return;
}