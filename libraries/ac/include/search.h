#ifndef DWC_SEARCH_H_
#define DWC_SEARCH_H_

u8   DWCi_AC_SearchAP         ( void );
void DWCi_AC_SetStealthChannel( u16 channel );
s8   DWCi_AC_GetStealthChannel( u16 no );
void DWCi_AC_SearchReStart    ( u8 phase );

#endif