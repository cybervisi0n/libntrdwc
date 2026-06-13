#include "util_wifiidtool_internal.h"

#define DWCUTIL_M_MACADDR  0x0000000000FFFFFF
#define DWCUTIL_M_RANDOM   0x000000000000FFFF
#define DWCUTIL_M_VENDER   0x0000000000000001
#define DWCUTIL_M_MAINUNIT 0x0000000000000003
#define DWCUTIL_M_UID      0x000007FFFFFFFFFF

#define DWCUTIL_E_MACADDR  0xFFFFFFFFFFFFFFFF
#define DWCUTIL_E_VENDER   0xFFFFFFFFFFFFFFFE
#define DWCUTIL_E_MAINUNIT 0xFFFFFFFFFFFFFFFD

typedef union tagUNI64{
	u64 data;
	u32 half[2];
	u8  i[8];
} DWCUtilUni64;

#define DWCUTIL_WIFIIDKEY1	(u8)0xd6
#define DWCUTIL_WIFIIDKEY2	(u8)0x67

const u8 DWCi_util_wifiid_ttable_inv[16]	= {0x07,0x02,0x05,0x0A,0x0B,0x00,0x0D,0x0F,0x0C,0x01,0x06,0x08,0x04,0x09,0x03,0x0E};
const u8 DWCi_util_wifiid_ttable[16]		= {0x05,0x09,0x01,0x0E,0x0C,0x02,0x0A,0x00,0x0B,0x0D,0x03,0x04,0x08,0x06,0x0F,0x07};
const u8 DWCi_util_wifiid_exctable[8]		= {1,2,0,4,3,5,6,7};

s32 DWCi_util_wifiid_sdayofyear[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

static s32	DWCi_Util_ConvertTimeToSecond(const RTCTime* time);
static s64	DWCi_Util_ConvertDateTimeToSecond(const RTCDate* date, const RTCTime* time);
static s32	DWCi_Util_ConvertDateToDay(const RTCDate* date);
static BOOL	DWCi_Util_IsLeapYear(u32 year);

static BOOL DWCi_Util_IsLeapYear(u32 year)
{
	return !(( year & 0x03 ));
}

static s32 DWCi_Util_ConvertDateToDay(const RTCDate* date)
{
	s32 dayNum;
	
	if(date->year >= 100
		|| (date->month < 1) || (date->month > 12)
		|| (date->day < 1) || (date->day > 31)
		|| (date->week >= RTC_WEEK_MAX)
		|| (date->month < 1) || (date->month > 12))
		return -1;
		
	dayNum = (s32)(date->day-1);
	dayNum += DWCi_util_wifiid_sdayofyear[date->month-1];
	if((date->month >= 3) && DWCi_Util_IsLeapYear(date->year))
		dayNum++;
	dayNum += date->year*365;
	dayNum += (date->year+3)/4;
	
	return dayNum;
}

static s32 DWCi_Util_ConvertTimeToSecond(const RTCTime* time)
{
	return (s32)((time->hour*60+time->minute)*60+time->second);
}

static s64 DWCi_Util_ConvertDateTimeToSecond(const RTCDate* date, const RTCTime* time)
{
	s32 day, sec;
	day = DWCi_Util_ConvertDateToDay(date);
    if(day == -1)
		return -1;
	sec = DWCi_Util_ConvertTimeToSecond(time);

	return ((s64)day)*(60*60*24)+sec;
}

static BOOL DWC_Util_CheckConfirmedWiFiId(void)
{
	DWCWiFiInfo	wifiinfo;
	

	DWCi_BM_GetWiFiInfo(&wifiinfo);
	
	if(wifiinfo.attestedUserId == 0)
		return FALSE;
	return TRUE;
}

static u64 DWCi_Util_WiFiId_scrambleUid(u16 rand,u32 mac, u8 vend, u8 mainunit)
{	
	DWCUtilUni64 uni64;
	u8 uidtemp[8];
	int i;

	uni64.data = ((u64)mainunit & DWCUTIL_M_MAINUNIT) | ((u64)vend & DWCUTIL_M_VENDER ) << 2| ((u64)mac & DWCUTIL_M_MACADDR ) << 3 | ((u64)rand & DWCUTIL_M_RANDOM) << 27 ; 

	for(i = 0; i < 6 ; i++ ){
		uni64.i[i] = (u8)(uni64.i[i] ^ DWCUTIL_WIFIIDKEY1);
	}
	

	for(i = 0; i < 5; i++){
		uni64.i[i] =(u8)( DWCi_util_wifiid_ttable[ (uni64.i[i] >> 4) & 0x0F ] << 4 |  DWCi_util_wifiid_ttable[ uni64.i[i] & 0x0F ]);
	}

	
	MI_CpuCopy8( uni64.i, uidtemp, 8);

	for(i = 0; i < 5 ; i++ ){
		uni64.i[DWCi_util_wifiid_exctable[i]] = uidtemp[i];
	}
	

	uni64.i[7] = 0;
	uni64.i[6] = 0;
	uni64.i[5] = (u8)(uni64.i[5] & 0x07);

	uni64.data = uni64.data << 1;

	uni64.i[0] = (u8)( uni64.i[0] | ( (uni64.i[5] >> 3) & 0x01) );

	for(i = 0; i < 6 ; i++ ){
		uni64.i[i] = (u8)(uni64.i[i] ^ DWCUTIL_WIFIIDKEY2);
	}

	uni64.i[7] = 0;
	uni64.i[6] = 0;
	uni64.i[5] = (u8)(uni64.i[5] & 0x07);
	
	return uni64.data;
}

static void DWCi_Util_WiFiId_unScrambleUid(u64 uid,u32* mac,u8* vend,u8* mainunit,u16* rand){

	u8 uidtemp[8];
	DWCUtilUni64 uni64;
	int i;
	
	uni64.data = uid;

	uni64.data = uni64.data & DWCUTIL_M_UID;
	

	for(i = 0; i < 6 ; i++ ){
		uni64.i[i] = (u8)(uni64.i[i] ^ DWCUTIL_WIFIIDKEY2);
	}
	

	uni64.data = uni64.data & DWCUTIL_M_UID;
	

	uni64.i[5] = (u8)(uni64.i[5] | ( (uni64.i[0] << 3) & 0x08));

	uni64.data = uni64.data >> 1;

	
	MI_CpuCopy8( uni64.i, uidtemp, 8);

	for(i = 0; i < 5 ; i++ ){
		uni64.i[i] = uidtemp[DWCi_util_wifiid_exctable[i]];
	}

	for(i = 0; i < 5; i++){
		uni64.i[i] = (u8)(DWCi_util_wifiid_ttable_inv[ (uni64.i[i] >> 4) & 0x0F ] << 4 |  DWCi_util_wifiid_ttable_inv[ uni64.i[i] & 0x0F ]);
	}
	

	for(i = 0; i < 6 ; i++ ){
		uni64.i[i] = (u8)(uni64.i[i] ^ DWCUTIL_WIFIIDKEY1);
	}
	
	*mac = (u32) (( uni64.data >> 3 ) & DWCUTIL_M_MACADDR);

	*vend =(u8 )(( uni64.data >> 2 ) & DWCUTIL_M_VENDER);

	*mainunit =(u8)(uni64.data  & DWCUTIL_M_MAINUNIT);
	
	*rand = (u16)((uni64.data >> 27 )& DWCUTIL_M_RANDOM);
}

BOOL DWCi_AUTH_GetNewWiFiInfo( DWCWiFiInfo* wifiinfo){
	
	u8  vend=0;
	u8  macaddr[6];
	u32 macUP3 = 0;
	u32 macDW3 = 0;
	s64 seed = 0;   
	u64 uid =  0;
	RTCDate rtcd;
	RTCTime time;
	MATHRandContext16 mrc16;
	

	DWCi_BM_GetWiFiInfo( wifiinfo );
	

	RTC_Init();
	if(RTC_GetDate(&rtcd) != 0) return FALSE;
	if(RTC_GetTime(&time) != 0) return FALSE;
	if( ( seed = DWCi_Util_ConvertDateTimeToSecond( &rtcd, &time ) ) < 0 )return FALSE;

	if(OS_IsTickAvailable()){
		seed += (u32)OS_GetTick;
	}
	
	

	MATH_InitRand16( &mrc16, (u32)seed );
	

	OS_GetMacAddress( macaddr );
	

	macUP3 = (u32)(macaddr[0] << 16 | macaddr[1] << 8 | macaddr[2]);
	vend = (u8)( macUP3 == 0x0009BF  ? 0 : 1 );

	macDW3 = (u32)(macaddr[3] << 16 | macaddr[4] << 8 | macaddr[5]);
	

	wifiinfo->pass = (u16)MATH_Rand16(&mrc16, 1000);
	

	wifiinfo->attestedUserId = 0;		
	

	if( wifiinfo->randomHistory == 0 ){
		
		wifiinfo->notAttestedId = 0;
		

		while( wifiinfo->notAttestedId == 0 ){
			

			MATH_Rand16(&mrc16, 0);
			

			while(mrc16.x == 0 )MATH_Rand16(&mrc16, 0);
			
			wifiinfo->randomHistory = (u16)mrc16.x;
			wifiinfo->notAttestedId = DWCi_Util_WiFiId_scrambleUid(wifiinfo->randomHistory, macDW3, vend,0);
		}
	
	}else{
		

		wifiinfo->notAttestedId = 0;
		

		while( wifiinfo->notAttestedId == 0){
		wifiinfo->randomHistory += 1;
		wifiinfo->notAttestedId = DWCi_Util_WiFiId_scrambleUid(wifiinfo->randomHistory, macDW3, vend,0);
		}
	}
	
	return TRUE;
}

BOOL DWCi_AUTH_MakeWiFiID( void* work){
		
	DWCWiFiInfo wifiinfo;
	
	if( !DWCi_AUTH_GetNewWiFiInfo(&wifiinfo) )return FALSE;
	

	return (DWCi_BM_SetWiFiInfo( &wifiinfo ,work) ? TRUE : FALSE );
}

BOOL DWCi_AUTH_UpDateWiFiID(DWCWiFiInfo* wifiinfo, void* work){
	

	DWCWiFiInfo wifiinfotemp;
	DWCi_BM_GetWiFiInfo( &wifiinfotemp );
	

	wifiinfo->attestedUserId = wifiinfo->notAttestedId;
	

	wifiinfo->notAttestedId = wifiinfotemp.notAttestedId;
	

	return (DWCi_BM_SetWiFiInfo( wifiinfo ,work) ? TRUE : FALSE );
}

BOOL DWCi_AUTH_RemakeWiFiID(DWCWiFiInfo* wifiinfo){
	
	u8  vend=0;
	u8  macaddr[6] ={0,};
	u32 macUP3 	= 0;
	u32 macDW3 	= 0;
	s64 seed 	= 0;   
	u64 uid 	= 0;
	RTCDate rtcd;
	RTCTime time;
	MATHRandContext16 mrc16;
	

	DWCi_BM_GetWiFiInfo( wifiinfo );
	

	RTC_Init();
	if(RTC_GetDate(&rtcd) != 0) return FALSE;
	if(RTC_GetTime(&time) != 0) return FALSE;
	if( ( seed = DWCi_Util_ConvertDateTimeToSecond( &rtcd, &time ) )< 0 )return FALSE;
	

	if(OS_IsTickAvailable()){
		seed += (u32)OS_GetTick;
	}
	

	MATH_InitRand16( &mrc16, (u32)seed );
		

	OS_GetMacAddress( macaddr );
	

	macUP3 = (u32)(macaddr[0] << 16 | macaddr[1] << 8 | macaddr[2]);
	vend = (u8)( macUP3 == 0x0009BF  ? 0 : 1 );
	

	macDW3 = (u32)(macaddr[3] << 16 | macaddr[4] << 8 | macaddr[5]);
	

	wifiinfo->pass = (u16)MATH_Rand16(&mrc16, 1000);
	
	wifiinfo->notAttestedId = 0;

	while( wifiinfo->notAttestedId == 0 ){

		MATH_Rand16(&mrc16, 0);
		

		while( ( mrc16.x == 0) || ( wifiinfo->randomHistory ==  (u16)mrc16.x ))MATH_Rand16(&mrc16, 0);
		wifiinfo->randomHistory = (u16)mrc16.x;
		

		wifiinfo->notAttestedId = DWCi_Util_WiFiId_scrambleUid(wifiinfo->randomHistory, macDW3, vend,0);
	}
	
	return TRUE;
}

void DWC_Auth_GetId( DWCAuthWiFiId* id ){
	
	DWCWiFiInfo wifiinfo;
	

	DWCi_BM_GetWiFiInfo( &wifiinfo );
	
	id->uId = wifiinfo.attestedUserId;
	id->notAttestedId = wifiinfo.notAttestedId;
	
	if( wifiinfo.attestedUserId == 0 ){
		id->flg = 0;
	}else{
		id->flg = 1;
	}
}

BOOL DWC_Auth_CheckPseudoWiFiID( void )
{
    DWCWiFiInfo wifiinfo;

    DWCi_BM_GetWiFiInfo( &wifiinfo );

    if ( wifiinfo.notAttestedId == 0 )
    {
        return FALSE;
    }

    return TRUE;
}

BOOL DWC_Auth_CheckWiFiIDNeedCreate( void )
{
    DWCWiFiInfo wifiinfo;

    DWCi_BM_GetWiFiInfo( &wifiinfo );

    if ( wifiinfo.notAttestedId == 0 && wifiinfo.attestedUserId == 0 )
    {
        return TRUE;
    }

    return FALSE;
}

