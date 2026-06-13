#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"


void DWCi_Ndi_getUserAgent(char *aUserAgentBuf)
{
  OS_SNPrintf(aUserAgentBuf, 0x21, "Nitro WiFi SDK/%d.%d", 2, 2);
  return;
}