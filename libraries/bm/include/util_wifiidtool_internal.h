#ifndef DWC_WIFIIDTOOL_INTERNAL_H_
#define DWC_WIFIIDTOOL_INTERNAL_H_

#include <nitro.h>
#include "bm/util_wifiidtool.h"
#include "../../bm/include/dwc_backup.h"

#ifdef __cplusplus
extern "C" {
#endif
BOOL DWCi_AUTH_MakeWiFiID( void* work);
BOOL DWCi_AUTH_UpDateWiFiID(DWCWiFiInfo* wifiinfo, void* work);
BOOL DWCi_AUTH_RemakeWiFiID(DWCWiFiInfo* wifiinfo);
BOOL DWCi_AUTH_GetNewWiFiInfo( DWCWiFiInfo* wifiinfo);
#ifdef __cplusplus
}
#endif

#endif
