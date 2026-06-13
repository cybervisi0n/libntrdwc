#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"


int DWCi_Ndi_intToStr(char *str,int num)
{
    return NHTTPi_intToStr(str,num);
}


int DWCi_Ndi_strToInt(char *str, int len)
{
    return NHTTPi_strToInt(str, len);
}
