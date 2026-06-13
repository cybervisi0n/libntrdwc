#include <nitro.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"
#include "ac_private.h"

void DWCi_AC_CallBackWCM(WCMNotice *notice)
{
	ACWORK *wk = DWCi_AC_GetMemPtr( ALLOC_NAME_WORK );

	if( notice->notify == WCM_NOTIFY_CONNECT )
	{
		if ( notice->result == WCM_RESULT_SUCCESS )
		{
			OS_Printf( "ConnectAP %s\n", ((WMBssDesc *)notice->parameter[0].p )->ssid );
		}

		else
		{
			
			switch( notice->parameter[ 1 ].n )
			{
			case 13: 
				
				
				wk->connectResult = AC_CONNECT_FAILURE_AUTH;
				break;
			case 15:
				
				
				wk->connectResult = AC_CONNECT_FAILURE_WEP;
				break;
			case 17:

				wk->connectResult = AC_CONNECT_FAILURE_CAPACITY;
				break;
			default:
				
				wk->connectResult = AC_CONNECT_FAILURE;
				break;
			}
			OS_Printf( "Failure! WCM_NOTIFY_CONNECT\n" );
		}
	}

	else if( notice->notify == WCM_NOTIFY_FOUND_AP )
	{

		DWCi_AC_GetBeacon( ( WMBssDesc *)notice->parameter[0].p );
	}

	else
	{
		if ( notice->result == WCM_RESULT_SUCCESS )
		{
			OS_Printf( "WCM Success! %d\n", notice->notify );
		}
		else
		{
			OS_Printf( "WCM Failure! %d\n", notice->notify );
		}
	}
}

