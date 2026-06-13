#ifndef DWC_SHOP_USB_H_
#define DWC_SHOP_USB_H_

#define NINTENDO_SHOP_SSID			"NDWCSHAP"
#define NINTENDO_SHOP_SSID_LENGTH	8
#define NINTENDO_USB_SSID			"NWCUSBAP"
#define NINTENDO_USB_SSID_LENGTH	8
#define NINTENDO_USB_CONNECT_FLAG	9

BOOL DWCi_AC_CheckNintendoShopAP( const char *ssid );

void DWCi_AC_GetNintendoShopWepKey( const char *ssid, u8 wep[13] );

void DWCi_AC_GetPostalCode( const char *ssid, u8 pos[10] );

BOOL DWCi_AC_CheckNintendoUsbAP( const char *ssid );

void DWCi_AC_GetNintendoUSBWepKey( const char *ssid, u8 wep[13] );

#endif