#ifndef  DWC_BACKUP_H
#define  DWC_BACKUP_H

#ifdef  __cplusplus
extern  "C" {
#endif

enum  tagDWCWepMode {
	DWC_WEP_MODE_NONE,
	DWC_WEP_MODE_40,
	DWC_WEP_MODE_104,
	DWC_WEP_MODE_128
};

enum  tagDWCSetType {
	DWC_SETTYPE_MANUAL,
	DWC_SETTYPE_AOSS,
	DWC_SETTYPE_RAKURAKU,
	DWC_SETTYPE_NOT       = 0xFF
};

#define  DWC_SET_WIFI_WORK_SIZE    0x400

typedef struct  tagDWCApInfo {
	u8  ispId    [0x20];
	u8  ispPass  [0x20];
	u8  ssid  [2][0x20];
	u8  wep   [4][0x10];
	u8  ip       [0x04];
	u8  gateway  [0x04];
	u8  dns   [2][0x04];
	u8  netmask;
	u8  wep2  [4][0x05];
	u8  authType;
	u8  wepMode      : 2;
	u8  wepNotation  : 6;
	u8  setType;
	u8  rsv      [0x07];
	u8  state;
}  DWCApInfo;

typedef struct  tagDWCWiFiInfo {
	u64  attestedUserId;
	u64  notAttestedId;
	u16  pass;
	u16  randomHistory;
}  DWCWiFiInfo;

typedef struct  tagDWCMemPage {
	DWCApInfo  ap;
	u8         wifi[0x0E];
	u16        crc;
}  DWCMemPage;

typedef struct  tagDWCMemMap {
	DWCMemPage  page[4];
}  DWCMemMap;

BOOL  DWCi_BM_GetApInfo(DWCMemPage* buf);

void  DWCi_BM_GetWiFiInfo(DWCWiFiInfo* buf);

BOOL  DWCi_BM_SetWiFiInfo(const DWCWiFiInfo* info, void* work);

#ifdef  __cplusplus
}
#endif

#endif
