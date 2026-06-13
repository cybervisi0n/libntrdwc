#ifndef  DWC_LOCAL_H
#define  DWC_LOCAL_H

#ifdef __cplusplus
extern "C" {
#endif

extern const u8  DWCi_SETTING_NONE[4];

typedef struct  tagMEMINIT {
	DWCMemMap         mm;
	u8                work[0x100];
	MATHCRC16Table    tbl;
}  MEMINIT;

typedef struct  tagMEMWRITEC {
	u8                work[0x100];
	MATHCRC16Table    tbl;
}  MEMWRITEC;

#define  DWC_WRITE_CHILD_WORK_SIZE    sizeof(MEMWRITEC)

BOOL  DWCi_BACKUPlInit(void* work);

BOOL  DWCi_BACKUPlRead(DWCMemMap* mem);

BOOL  DWCi_BACKUPlWritePage(const DWCMemMap* data, const BOOL* page
						  , void* work);

BOOL  DWCi_BACKUPlWriteAll(MEMINIT* work);

BOOL  DWCi_BACKUPlWriteChild(DWCMemMap* data, void* work);

void  DWCi_BACKUPlSetWiFi(const u8* wifi);

u8  DWCi_BACKUPlConvMaskCidr(const u8* mask);

void  DWCi_BACKUPlConvMaskAddr(int mask, u8* buf);

BOOL  DWC_BACKUPlCheckSsid(const u8* ssid);

BOOL  DWC_BACKUPlCheckIp(const u8* ip, const u8* mask);

BOOL  DWC_BACKUPlCheckAddress(const u8* address);

const u8*  DWCi_BACKUPlConvWifiInfo(const DWCWiFiInfo* info);

const void* DWCi_BACKUPlGetWifi();

#ifdef  __cplusplus
}
#endif

#endif
