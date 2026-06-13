#include <nitro.h>
#include <string.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"
#include "ac_private.h"

#include "base/dwc_report.h"


enum{
	SSID_NO_USER_NORMAL,
	SSID_NO_USER_AOSS
};
#define	FREESPOT_SSID_LENGTH	8
#define	WAYPORT_SSID_LENGTH		8
#define NINTENDOWFC_SSID_LENGTH	11
static const u8 FREESPOT_SSID[ FREESPOT_SSID_LENGTH ]       = { "FREESPOT" };
static const u8 WAYPORT_SSID[ WAYPORT_SSID_LENGTH ]         = { "Wayport2" };
static const u8 NINTENDOWFC_SSID[ NINTENDOWFC_SSID_LENGTH ] = { "NINTENDOWFC" };

typedef int undefined;

static uint MakeAroundList(tagACWORK * aAcWork);
static u8 MakeStealthList( tagACWORK *wk );
static u8 MakeUserList( tagACWORK *wk );
static BOOL iCopySSID(int no,DWCMemPage *info,ACSL *list,int base_type);
static u8 MakeDifferChannelList(tagACWORK *wk);
static uint CheckDifferChannelStart(void * param_1);


u8 DWCi_AC_MakeSearchList( int type )
{
  undefined uVar1;
  int iVar2;
  tagACWORK * myAcWork;
  
  myAcWork = DWCi_AC_GetMemPtr(0x10);
  if (type == DWCi_AC_MakeSearchList_Around) {
    //MI_CpuClear32(myAcWork + 0x300,0x144);
    MI_CpuClear32(myAcWork->searchList, 0x144);
    uVar1 = MakeAroundList(myAcWork);

    myAcWork->searchListNum = uVar1;

    DWC_Printf(DWC_REPORTFLAG_AC,"  ==========     AROUND SEARCH LIST     ==========\n");
  } else if (type == DWCi_AC_MakeSearchList_DifferChannel) {
    uVar1 = MakeDifferChannelList(myAcWork);

    myAcWork->searchListNum = uVar1;
    uVar1 = CheckDifferChannelStart(myAcWork);

    myAcWork->searchListNo = uVar1;
    DWC_Printf(DWC_REPORTFLAG_AC,"  ========== DIFFER CHANNEL SEARCH LIST ==========\n");
  } else if (type == DWCi_AC_MakeSearchList_Stealth) {

    MI_CpuClear32(myAcWork->searchList, 0x144);

    myAcWork->searchListNo = 0;
    uVar1 = MakeStealthList(myAcWork);

    myAcWork->searchListNum = uVar1;
    DWC_Printf(DWC_REPORTFLAG_AC,"  ==========     STEALTH SEARCH LIST    ==========\n");
  }
  //DWC_Printf(0x2000000,"  User SetAP1 %s \n",iVar2 + 0x40);
  DWC_Printf(DWC_REPORTFLAG_AC,"  User SetAP1 %s \n",myAcWork->userInfo[0].ap.ssid[0]);
  //DWC_Printf(0x2000000,"  User SetAP2 %s \n",iVar2 + 0x140);
  DWC_Printf(DWC_REPORTFLAG_AC,"  User SetAP2 %s \n",myAcWork->userInfo[1].ap.ssid[0]);
  //DWC_Printf(0x2000000,"  User SetAP3 %s \n",iVar2 + 0x240);
  DWC_Printf(DWC_REPORTFLAG_AC,"  User SetAP3 %s \n",myAcWork->userInfo[2].ap.ssid[0]);
  DWC_Printf(DWC_REPORTFLAG_AC,"  ------------------------------------------------\n");
  DWC_Printf(DWC_REPORTFLAG_AC,"  SearchAP type %2d %s\n", myAcWork->searchList[0].type,myAcWork->searchList[0].ssid);
  DWC_Printf(DWC_REPORTFLAG_AC,"  SearchAP type %2d %s\n", myAcWork->searchList[1].type,myAcWork->searchList[1].ssid);
  DWC_Printf(DWC_REPORTFLAG_AC,"  SearchAP type %2d %s\n", myAcWork->searchList[2].type,myAcWork->searchList[2].ssid);
  DWC_Printf(DWC_REPORTFLAG_AC,"  SearchAP type %2d %s\n", myAcWork->searchList[0].type,myAcWork->searchList[3].ssid);
  DWC_Printf(DWC_REPORTFLAG_AC,"  SearchAP type %2d %s\n", myAcWork->searchList[0].type,myAcWork->searchList[4].ssid);
  DWC_Printf(DWC_REPORTFLAG_AC,"  SearchAP type %2d %s\n", myAcWork->searchList[0].type,myAcWork->searchList[5].ssid);
  DWC_Printf(DWC_REPORTFLAG_AC,"  SearchAP type %2d %s\n", myAcWork->searchList[0].type,myAcWork->searchList[6].ssid);
  DWC_Printf(DWC_REPORTFLAG_AC,"  SearchAP type %2d %s\n", myAcWork->searchList[0].type,myAcWork->searchList[7].ssid);
  DWC_Printf(DWC_REPORTFLAG_AC,"  SearchAP type %2d %s\n", myAcWork->searchList[0].type,myAcWork->searchList[8].ssid);
  DWC_Printf(DWC_REPORTFLAG_AC,"  ================================================\n");
  return myAcWork->searchListNum;
}

u8 DWCi_AC_CheckFreespot(WMBssDesc *bssdesc)
{
  u8 uVar1;
  tagACWORK *wk;
  int iVar2;
  
  wk = (tagACWORK *)DWCi_AC_GetMemPtr(0x10);
  if ((((wk->connectType & 0xf) == 0) || ((wk->connectType & 0xf) == 6)) &&
     (iVar2 = strncmp((char *)bssdesc->ssid,(char *)"FREESPOT",8), iVar2 == 0)) {
    uVar1 = '\b';
  }
  else {
    uVar1 = '\0';
  }
  return uVar1;
}


static uint MakeAroundList(tagACWORK * wk)
{
	u8   num   = 0;
	ACSL *list = wk->searchList;

	num = MakeUserList( wk );
	list += num;

	if( wk->connectType == 0 || wk->connectType == DWC_AC_AP_TYPE_FREESPOT + 1 )
	{
		MI_CpuCopy8( FREESPOT_SSID, list->ssid, FREESPOT_SSID_LENGTH );
		list->length = FREESPOT_SSID_LENGTH;
		list->type   = AP_TYPE_FREESPOT;
		num++;
	}

	return num;
}


static u8 MakeStealthList( tagACWORK *wk )
{
	u8   num   = 0;
	ACSL *list = wk->searchList;

	num = MakeUserList( wk );
	list += num;

	if( wk->connectType == 0 || wk->connectType == DWC_AC_AP_TYPE_USB + 1 )
	{
		MI_CpuCopy8( NINTENDO_USB_SSID, list->ssid, NINTENDO_USB_SSID_LENGTH );
		list->length = NINTENDO_USB_SSID_LENGTH;
		list->type   = AP_TYPE_USB;
		num++;
		list++;
	}

	if( wk->connectType == 0 || wk->connectType == DWC_AC_AP_TYPE_WAYPORT + 1 )
	{
		MI_CpuCopy8( WAYPORT_SSID, list->ssid, WAYPORT_SSID_LENGTH );
		list->length = WAYPORT_SSID_LENGTH;
		list->type   = AP_TYPE_WAYPORT;
		num++;
		list++;
	}

	if( wk->connectType == 0 || wk->connectType == DWC_AC_AP_TYPE_NINTENDOWFC + 1 )
	{
		MI_CpuCopy8( NINTENDOWFC_SSID, list->ssid, NINTENDOWFC_SSID_LENGTH );
		list->length = NINTENDOWFC_SSID_LENGTH;
		list->type   = AP_TYPE_NINTENDOWFC;
		num++;
	}

	return num;
}


static u8 MakeUserList( tagACWORK *wk )
{
	int        i;
	u8         num   = 0;
	DWCMemPage *info = wk->userInfo;
	ACSL       *list = wk->searchList;

	for( i = 0 ; i < 3 ; i++ )
	{

		if( wk->connectType == 0 || wk->connectType == DWC_AC_AP_TYPE_USER1 + 1 + i )
		{
			if( info->ap.setType != 0xFF )
			{
				if( iCopySSID( SSID_NO_USER_NORMAL, info, list, AP_TYPE_USER1 + i ) != FALSE )
				{
					num++;
					list++;
				}
				if( info->ap.setType == 1 )
				{
					if( iCopySSID( SSID_NO_USER_AOSS, info, list, AP_TYPE_AOSS_USER1 + i ) != FALSE )
					{
						num++;
						list++;
					}
				}
			}
		}
		info++;
	}
	return num;
}


static BOOL iCopySSID(int no,DWCMemPage *info,ACSL *list,int base_type)
{
	u8 j;
	for( j = 0 ; j < WM_SIZE_SSID ; j++ )
	{
		if( info->ap.ssid[ no ][ j ] == 0 ) break;
		list->ssid[ j ] = info->ap.ssid[ no ][ j ];
	}
	if( j != 0)
	{
		list->length = j;
		list->type   = (u8)base_type;
		return TRUE;
	}
	return FALSE;
}



static u8 MakeDifferChannelList(tagACWORK *wk)
{
  u8 uVar1;
  undefined1 *puVar2;
  int iVar3;
  
  uVar1 = '\0';
  //for (iVar3 = 0; iVar3 < (int)(uint)wk->findListNum; iVar3 = iVar3 + 1) {
  //  if ((wk->findList[iVar3].state == '\0') &&
  //     (wk->bssDesc[iVar3].channel - 1 != ((byte)wk->findList[iVar3].field_0x3 & 0x7f))) {
  //    //puVar2 = &wk->findList[iVar3].field_0x3;
  //    //*puVar2 = *puVar2 & 0x7f;
  //    //uVar1 = uVar1 + '\x01';
  //  }
  //  else {
  //    //puVar2 = &wk->findList[iVar3].field_0x3;
  //    //*puVar2 = *puVar2 & 0x7f | 0x80;
  //  }
  //}
  return uVar1;
}

static uint CheckDifferChannelStart(void * param_1)
{
  uint uVar1;
  uint uVar2;
  
  uVar2 = 0;
  while ((uVar1 = 0, uVar2 < *(byte *)(param_1 + 0xd12) &&
         (uVar1 = uVar2, (int)((uint)*(byte *)(param_1 + uVar2 * 4 + 0x447) << 0x18) < 0))) {
    uVar2 = uVar2 + 1 & 0xff;
  }
  return uVar1;
}