#ifndef DWC_AC_PRIVATE_H_
#define DWC_AC_PRIVATE_H_

#include <nitro.h>
#include <nitroWiFi.h>
#include <string.h>
#include <ac/dwc_ac.h>
#include <auth/dwc_netcheck.h>
#include <base/dwc_report.h>
#include "shop_usb.h"
#include "start.h"
#include "search.h"
#include "connect.h"
#include "retry.h"
#include "test.h"
#include "callback.h"
#include "makelist.h"
#include "beacon.h"
#include "close.h"
#include "error.h"

#define 	DWC_AC_CONNECT_RETRY_TIME		3

#define 	DWC_AC_GET_IP_ADDRESS_TIME_OUT	10

#define		ALLOC_NAME_HOLD					0x0001
#define		ALLOC_NAME_WCM_BUFFER			0x0002
#define		ALLOC_NAME_SOC_CONFIG			0x0004
#define		ALLOC_NAME_NET_CHECK			0x0008
#define		ALLOC_NAME_WORK					0x0010

#define		SEARCH_AP_LIST_NUM				9

#define		FOUND_AP_LIST_NUM				10

#define		SEARCH_AROUND_TIME_OUT			300
#define		SEARCH_CHANNEL_TIME_OUT			150
#define		SEARCH_STEALTH_TIME_OUT			150

#define		CONNECT_PERMIT_RSSI_POWER		20

#define		DWC_AC_SEARCH_START_CHANNEL1	0
#define		DWC_AC_SEARCH_START_CHANNEL2	1

#define		AROUND_SEARCH_NUM_MAX			1

#define		DWC_AC_ERROR_DISCONNECT			-50000
#define		DWC_AC_ERROR_NO_AP_AROUND		-50099
#define		DWC_AC_ERROR_NO_MACTH_SSID		-51099
#define		DWC_AC_ERROR_WEP_MISS			-51100
#define		DWC_AC_ERROR_OVER_CAPACITY  	-51200
#define		DWC_AC_ERROR_NOT_CONNECT		-51300
#define		DWC_AC_ERROR_DHCP				-52000
#define		DWC_AC_ERROR_NETCHECK_DNS		-52100
#define		DWC_AC_ERROR_NETCHECK_1			-52200
#define		DWC_AC_ERROR_NETCHECK_2			-52300
#define		DWC_AC_ERROR_AUTH_1				-53000
#define		DWC_AC_ERROR_AUTH_2				-53100
#define		DWC_AC_ERROR_AUTH_3				-53200

enum{
	AC_PHASE_NULL,
	AC_PHASE_START,
	AC_PHASE_SEARCH_START,
	AC_PHASE_SEARCH_AROUND,
	AC_PHASE_SEARCH_DIFFER_CHANNEL,
	AC_PHASE_SEARCH_STEALTH,
	AC_PHASE_SEARCH_END,
	AC_PHASE_CONNECT_START,
	AC_PHASE_CONNECT_AP,
	AC_PHASE_CONNECT_RETRY,
	AC_PHASE_TEST_START,
	AC_PHASE_TEST_RETRY,
	AC_PHASE_TEST_GET_IP,
	AC_PHASE_TEST_CONNECT_CREATE,
	AC_PHASE_TEST_CONNECT_PROCESS,
	AC_PHASE_TEST_END,
	AC_PHASE_COMPLETE,
	AC_PHASE_ERROR_OCCURED,
	AC_PHASE_ERROR
};

enum{
	AC_CONNECT_START,
	AC_CONNECT_FAILURE_AUTH,
	AC_CONNECT_FAILURE_WEP,
	AC_CONNECT_FAILURE_CAPACITY,
	AC_CONNECT_FAILURE
};

enum{
	AC_ERROR_WCM_FATAL,
	AC_ERROR_WCM_START_UP,
	AC_ERROR_SOC_START_UP,
	AC_ERROR_NETCHECK_CREATE,
	AC_ERROR_WCM_IRREGULAR,
	AC_ERROR_NOT_FOUND_AP,
	AC_ERROR_NOT_FOUND_INET
};

enum{
	SEARCH_LIST_AROUND,
	SEARCH_LIST_DIFFER_CHANNEL,
	SEARCH_LIST_STEALTH,
	SEARCH_LIST_USB,
	SEARCH_LIST_WAYPORT
};

enum{
	AP_TYPE_USER1,
	AP_TYPE_USER2,
	AP_TYPE_USER3,
	AP_TYPE_AOSS_USER1,
	AP_TYPE_AOSS_USER2,
	AP_TYPE_AOSS_USER3,
	AP_TYPE_USB,
	AP_TYPE_SHOP,
	AP_TYPE_FREESPOT,
	AP_TYPE_WAYPORT,
	AP_TYPE_NINTENDOWFC
};

enum{
	AP_CONNECT_OK,
	AP_CONNECT_NG,
	AP_DISCONNECT,
	AP_WEP_MISS,
	AP_OVER_CAPACITY
};

void  DWCi_AC_InsertApInfo  ( int no, DWCBmApInfo *info );
u8    DWCi_AC_GetPhase      ( void );
void *DWCi_AC_Alloc         ( u32 name, s32 size );
void  DWCi_AC_Free          ( u32 name, void *ptr, s32 size );
void  DWCi_AC_FreeAll       ( void );
void  DWCi_AC_SetPhase      ( u8 phase );
void *DWCi_AC_GetMemPtr     ( u32 name );
void  DWCi_AC_SetError      ( int error );
int   DWCi_AC_GetError      ( void );
void  DWCi_AC_SetApType     ( u8 type );
u8    DWCi_ConvConnectAPType( u8 type );

extern u8* WCM_GetApMacAddress( void );
extern u8* WCM_GetApEssid     ( u16* length );

#endif
