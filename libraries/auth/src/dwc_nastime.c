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

#include <nitro/rtc.h>

#ifdef SDK_BUILD_LINUX
#include <errno.h>
#include <wchar.h>
#endif

s64	DWCnastimediff;
BOOL	DWCnastimediffvalid;
s64     DWCnastimediffbase;


static const char * monthname[] = {
  "Jan",
  "Feb",
  "Mar",
  "Apr",
  "May",
  "Jun",
  "Jul",
  "Aug",
  "Sep",
  "Oct",
  "Nov",
  "Dec"
};


u32 DWC_Auth_SetNasTimeDiff(char *aTime)
{
  s64 sVar1;
  size_t sVar2;
  size_t sVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  longlong lVar7;
  longlong lVar8;
  char acStack_50 [50];
  char acStack_4b [2];
  undefined1 local_49;
  char acStack_48 [3];
  undefined1 local_45;
  char acStack_44 [4];
  undefined1 local_40;
  char acStack_3f [2];
  undefined1 local_3d;
  char acStack_3c [2];
  undefined1 local_3a;
  char acStack_39 [2];
  undefined1 local_37;
  RTCTime local_30;
  RTCDate local_24;
  
  sVar2 = strlen(aTime);
  sVar3 = strlen("Fri, 03 Mar 2006 01:28:13 GMT");
  if ((((sVar2 == sVar3) && (aTime[7] == ' ')) && (aTime[0xb] == ' ')) &&
     (((aTime[0x10] == ' ' && (aTime[0x13] == ':')) &&
      ((aTime[0x16] == ':' && (aTime[0x19] == ' ')))))) {
    DWC_Printf(0x1000000,"\tDate string from NAS is %s\n",aTime);
    iVar4 = RTC_GetDateTime(&local_24,&local_30);
    if (iVar4 == 0) {
      lVar7 = RTC_ConvertDateTimeToSecond(&local_24,&local_30);
      if (lVar7 != -1) {
        strcpy(acStack_50,aTime);
        local_49 = 0;
        local_45 = 0;
        local_40 = 0;
        local_3d = 0;
        local_3a = 0;
        local_37 = 0;
        errno = 0;
        local_24.year = atoi(&acStack_50[12]);
        if (errno != 0x22) {
          local_24.year = local_24.year - 2000;
          local_24.month = 0xd;
          for (iVar4 = 0; iVar4 < 0xc; iVar4 = iVar4 + 1) {
            iVar5 = strncmp(monthname[iVar4],&acStack_50[8], 3);
            if (iVar5 == 0) {
              local_24.month = iVar4 + 1;
              break;
            }
          }
          if (local_24.month < 0xd) {
            errno = 0;
            local_24.day = atoi(&acStack_50[5]);
            if (errno != 0x22) {
              errno = 0;
              local_30.hour = atoi(&acStack_50[17]);
              if (errno != 0x22) {
                errno = 0;
                local_30.minute = atoi(&acStack_50[20]);
                if (errno != 0x22) {
                  errno = 0;
                  local_30.second = atoi(&acStack_50[23]);
                  if (errno != 0x22) {
                    lVar8 = RTC_ConvertDateTimeToSecond(&local_24,&local_30);
                    if (lVar8 != -1) {
                      //DWCnastimediff = (u32)((uint)lVar7 - (uint)lVar8) |
                      //((u64)((int)((ulonglong)lVar7 >> 0x20) -
                      //     ((int)((ulonglong)lVar8 >> 0x20) + (uint)((uint)lVar7 < (uint)lVar8))) >> 32);
                      DWCnastimediff = lVar7 - lVar8;
                      DWCnastimediffvalid = 1;
                      DWCnastimediffbase = lVar7;
                      DWC_Printf(0x1000000,"\tDWC_Auth_SetNasTimeDiff : Success\n");
                      uVar6 = 1;
                      sVar1 = DWCnastimediffbase;
                      goto LAB_000103b4;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  DWC_Printf(0x1000000,"\tDWC_Auth_SetNasTimeDiff : Fail\n");
  //sVar1 = CONCAT44(DWCnastimediffbase._4_4_,(undefined4)DWCnastimediffbase);
  //uVar6 = 0;
LAB_000103b4:
  //DWCnastimediffbase._4_4_ = (undefined4)((ulonglong)sVar1 >> 0x20);
  //DWCnastimediffbase._0_4_ = (undefined4)sVar1;
  //DWCnastimediffbase = sVar1;
  return uVar6;
}


