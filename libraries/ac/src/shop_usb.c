#include <nitro.h>
#include <nitroWiFi.h>
#include "ac/dwc_ac.h"
#include "decomp/decomp_defs.h"
#include "shop_usb.h"

#define WEPKEY_LENGTH				13
#define SSID_LENGTH					24

static const char *MASKkey0   = "gwi'6&fs=0Nf~";
static const char *MASKkey1   = "%(egEr)ag(s&m";

static const char *SHMASKkey0 = "38g6zxjk20gvmv]6^=j&%vY1";
static const char *SHMASKkey1 = "952uybjnpmu903bia@bk5m[-";

static const u8 CONV_4BIT_TABLE[16] = {10,13,14,8,9,3,6,0,12,5,2,7,11,1,15,4};
static const u8 CONV_BYTE_TABLE[13] = {5,1,12,4,2,3,10,0,11,7,9,8,6};

static const char *base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

typedef union tagU4B{
	u32	us32;
	u8	us8[4];
}U4B;

static void decodeSSID(const char ssid[32], u8 buf[24]);
static void makeShopWepKey( const u8* seed, u8 * wepkey);
static void makeUsbWepKey( const u8* seed, u8 * wepkey);
static int  eb64( const u8* src , char *dest, u32 srcsize, u32 destsize );
static void enclode_char(u32 bb, int srclen, u8 *dest, int j);
static void enclode(const u8 *src, u8 *dest);
static long codetovalue(u8 c);
static int  db64( const char *src, u8 *dest, u32 srcsize, u32 destsize );

BOOL DWCi_AC_CheckNintendoShopAP( const char *ssid )
{
	u8 buf[24];

	decodeSSID( ssid, buf );

	if ( !memcmp( buf, NINTENDO_SHOP_SSID, NINTENDO_SHOP_SSID_LENGTH ))	return TRUE;
	return FALSE;
}

void DWCi_AC_GetNintendoShopWepKey( const char *ssid, u8 wep[13] )
{
	u8 buf[24];

	decodeSSID( ssid, buf );

	makeShopWepKey( buf, wep );
}

void DWCi_AC_GetPostalCode( const char *ssid, u8 pos[10] )
{
	u8 buf[24];

	decodeSSID( ssid, buf );

	if ( !memcmp( buf, NINTENDO_SHOP_SSID, NINTENDO_SHOP_SSID_LENGTH ))
	{

		MI_CpuCopy8( (u8 *)&buf[ NINTENDO_SHOP_SSID_LENGTH ], pos, 10 );
	}

}

BOOL DWCi_AC_CheckNintendoUsbAP( const char *ssid )
{

	if ( !memcmp( ssid, NINTENDO_USB_SSID, NINTENDO_USB_SSID_LENGTH ))
	{
		return TRUE;
	}
	return FALSE;
}

void DWCi_AC_GetNintendoUSBWepKey( const char *ssid, u8 wep[13] )
{

	ssid += 12;

	makeUsbWepKey( (const u8 *)ssid, wep );
}

static void decodeSSID(const char ssid[32], u8 buf[24])
{

	int i;
	u8 ctable_inv[] = {23,20,17,13,11,6,15,14,9,21,12,4,2,1,18,16,5,3,19,10,7,8,0,22};
	

	int dsize = db64( ssid, buf, 32, 24 );

	for( i = 0; i < SSID_LENGTH ; i++ )
	{
		buf[i] = (u8)( buf[i] ^ SHMASKkey1[i] );
	}

	for(i = 0 ; i  < SSID_LENGTH ; i++ )
	{
		u8 destnum=(u8)i;
		u8 dest;
		u8 srcnum = (u8)i;
		u8 src = buf[i];

		while( ctable_inv[srcnum] != 0xFF )
		{
			dest                     = buf[ctable_inv[srcnum]];
			buf[ctable_inv[destnum]] = src;
			destnum                  = ctable_inv[srcnum];
			ctable_inv[srcnum]       = 0xFF;
			src                      = dest;
			srcnum                   = destnum;
		}
	}

	for( i = 0 ; i < SSID_LENGTH ; i++ )
	{
		buf[i] = (u8)( buf[i] ^ SHMASKkey0[i] );
	}
}

static void makeShopWepKey( const u8* seed, u8 * wepkey)
{
	u8 temp[17];
	u8 md5src[3] = {0x61,0x61,0x61};
	MATHMD5Context context;

	DWC_Printf( DWC_REPORTFLAG_AC,  "md5sum_check = \n");

	MATH_MD5Init( &context );
	MATH_MD5Update( &context, seed, 24);
	MATH_MD5GetHash( &context, temp);

	MI_CpuCopy8( &(temp[3]),wepkey,13);
}

static void makeUsbWepKey( const u8* seed, u8 * wepkey)
{
	int i;
	u8 keytemp[ WEPKEY_LENGTH ];

	for( i = 0 ; i < WEPKEY_LENGTH ; i++ )
	{
			wepkey[i] = (u8)(seed[i] ^ seed[WEPKEY_LENGTH + i % 7]);
	}

	for( i = 0 ; i < 7 ; i++ )
	{
		wepkey[3 + i] = (u8)(wepkey[3 + i] ^ seed[WEPKEY_LENGTH + i]);
	}
	

	for( i = 0 ; i < WEPKEY_LENGTH ; i++ )
	{
		wepkey[i] = (u8)(wepkey[i] ^ MASKkey0[i]);
	}

	MI_CpuCopy8( wepkey, keytemp, WEPKEY_LENGTH );
	for( i = 0 ; i < WEPKEY_LENGTH ; i++ )
	{
		wepkey[CONV_BYTE_TABLE[i]]=keytemp[i];
	}

	for ( i = 0 ; i < WEPKEY_LENGTH ; i++ )
	{
		wepkey[i] = (u8)(wepkey[i] ^ MASKkey1[i]);
	}
	

	for ( i = 0 ; i < WEPKEY_LENGTH ; i++ )
	{
		wepkey[i] = (u8)(CONV_4BIT_TABLE[(wepkey[i] >> 4) & 0x0F] << 4 | CONV_4BIT_TABLE[wepkey[i] & 0x0F] );
	}

	for( i = 0 ; i < 3 ; i++ )
	{
		wepkey[i]     = (u8)(wepkey[i]     ^ wepkey[i + 6]);
		wepkey[i + 3] = (u8)(wepkey[i + 3] ^ wepkey[i + 9]);
		wepkey[i + 6] = (u8)(wepkey[i + 6] ^ wepkey[i + 3]);
		wepkey[i + 9] = (u8)(wepkey[i + 9] ^ wepkey[i]);
		wepkey[12]    = (u8)(wepkey[12]    ^ wepkey[i]);
	}
}

static int eb64( const u8* src , char *dest, u32 srcsize, u32 destsize )
{	
	u32 srcmod = 0;
	u32 srcmax = 0;
	U4B bb;
	int i,j,base,k;
	

	if( destsize >= (srcsize/3)*4 + (srcsize%3 ? srcsize%3 + 1 : 0) ) {	
		srcmod = srcsize % 3;
			srcmax = srcsize - srcmod;	
	}else{
		return -1;
	}

	for( i = 0; i < srcmax ; i+=3){

		for(j = 0 ; j < 3 ; j++){
			bb.us8[3 - j]=src[i + j];
		}

		for ( base = 26,k = 0; k < 4; k++, base -= 6) {
			dest[(i/3)*4+k] =  base64[ (u32)( ( bb.us32 >> base) & 0x3F) ];
		}
	}

	if( srcmod != 0 ){
	bb.us32 = 0;
	dest[ (srcmax/3)*4]='=';
	for( i = 0 ; i < srcmod ; i++){
		bb.us8[3 - i]=src[ srcmax+i];
		dest[ (srcmax/3)*4+i+1] = '=';
	}
	for (base = 26, k = 0; k < srcmod + 1; k++, base -= 6) {

		dest[ (srcmax/3)*4 + k] = (s8)base64[ (u32)((bb.us32 >> base) & 0x3F) ];
	}
	}
	return (int)( (srcsize/3)*4 + ( srcsize%3 ? srcsize%3 + 1 : 0 ));
}

static void enclode_char(u32 bb, int srclen, u8 *dest, int j)
{
	int	x, i, base;

	
	for ( i = srclen; i < 2; i++ ) 
		bb <<= 8;

	
	for ( base = 18, x = 0; x < srclen + 2; x++, base -= 6) {
		dest[j++] = (u8)base64[ (u32)((bb>>base) & 0x3F) ];
	}

	
	for ( i = x; i < 4; i++ ) {
		dest[j++] = (u8)'=';		
	}

}

static void enclode(const u8 *src, u8 *dest)
{
	unsigned	char	*p = (u8 *)src;
	unsigned	long	bb = (u32)0;
	int					i = 0, j = 0;

	while (*p)
	{
		bb <<= 8;
		bb |= (u32)*p;

		
		if (i == 2) {
			enclode_char(bb, i, dest, j);

			j = j + 4;
			i = 0;
			bb = 0;
		} else
			i++;

		p++;
	}

	
	if (i)
		enclode_char(bb, i - 1, dest, j);

}

static long codetovalue(u8 c)
{
	if( (c >= (u8)'A') && (c <= (u8)'Z') ) {
		return (long)(c - (u8)'A');
	}
	else if( (c >= (u8)'a') && (c <= (u8)'z') ) {
		return ((long)(c - (u8)'a') +26);
	}
	else if( (c >= (u8)'0') && (c <= (u8)'9') ) {
		return ((long)(c - (u8)'0') +52);
	}
	else if( (u8)'+' == c ) {
		return (long)62;
	}
	else if( (u8)'/' == c ) {
		return (long)63;
	}
	else if( (u8)'=' == c ) {
		return (long)0;
	}
	else {
		return -1;
	}
}

static int db64( const char *src, u8 *dest, u32 srcsize, u32 destsize )
{
	U4B u4b;
	u32 x;
	u32 base64 = 0;
	int srcmod = 0;
	int srcmax = 0;
	int i,j,k;

	if( destsize >= ( srcsize * 3 ) / 4 ){	
		srcmod = (int)(srcsize % 4);
		srcmax = (int)(srcsize - srcmod);	
	}else{
		return -1;
	}

	for (i=0; i<srcmax;i+=4){
		base64 = 0;

		for(j=0; j < 4;j++){
			x = (u32)codetovalue((u8)src[i+j]);
			base64 |= x << ( 3 - j )*6;
		}

		u4b.us32 = base64;
		for(k=0; k < 3; k++ ){
			dest[ (i/4)*3 + k ] = u4b.us8[ 2 - k ];
		}
	}

	if( srcmod ) {
		u4b.us32=0;
		base64 = 0;
		for ( i = 0 ; i < srcmod; i++)
		{
			x = (u32)codetovalue((u8)src[ srcmax + i ]);
			base64 |= x << ( 3 - i )*6;
			u4b.us32 = u4b.us32 | base64;
		}
		for( k=0; k < srcmod; k++ ){
			dest[ (srcmax*3)/4 + k ] = u4b.us8[ 2 - k ];
		}
	}
	return (int)(( srcsize * 3 ) / 4 );
}
