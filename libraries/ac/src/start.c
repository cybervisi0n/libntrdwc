#include <nitro.h>
#include <nitroWiFi.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"
#include "ac_private.h"

u8 DWCi_AC_Start( void )
{
	IWConfig  config;
	int       result;
	int       phase = WCM_GetPhase();
	ACWORK   *wk    = DWCi_AC_GetMemPtr( ALLOC_NAME_WORK );

	if( phase == WCM_PHASE_WAIT )
	{

		config.dmano     = wk->dmaNo;
		config.pbdbuffer = NULL;
		config.nbdbuffer = 0;
		config.nbdmode   = 0;

        (void)DWCi_AC_MakeSearchList( SEARCH_LIST_AROUND );

        result = WCM_StartupAsync( &config, DWCi_AC_CallBackWCM );

    if( result == WCM_RESULT_FAILURE || result >= WCM_RESULT_REJECT )
		{

			DWC_Printf( DWC_REPORTFLAG_AC,   "WCM_StartupAsync Failuer result is %d\n", result );
			DWCi_AC_SetError( AC_ERROR_WCM_START_UP );
			return AC_PHASE_ERROR_OCCURED;
		}
	}

	else
	{
		return AC_PHASE_START;
	}
	return AC_PHASE_SEARCH_START;
}