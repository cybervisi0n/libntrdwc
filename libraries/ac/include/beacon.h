#ifndef DWC_BEACON_H_
#define DWC_BEACON_H_

#ifdef __cplusplus
extern "C" {
#endif

void DWCi_AC_GetBeacon        ( WMBssDesc *bssdesc );
int  DWCi_AC_CheckNintendoSSID( WMBssDesc *bssdesc );

#ifdef __cplusplus
}
#endif

#endif
