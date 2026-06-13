#include <nitro.h>
#include <dwc.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"


static int SslAuthCallback(int param_1, CPSSslConnection *con, int param_3);


static int SslAuthCallback(int param_1, CPSSslConnection *con, int param_3)
{
  //NHTTPi_DBGOUT_ARG("SSL: %d s:<%s>\n",param_3,con->subject);
  //NHTTPi_DBGOUT_ARG("SSL:     /CN=%s/\n",con->cn);
  //NHTTPi_DBGOUT_ARG("SSL:   i:<%s>\n",con->issuer);
  if ((param_1 & 0x8000U) != 0) {
    NHTTPi_DBGOUT_STR_("SSL: Certificate is out-of-date\n");
    param_1 = param_1 & 0xffff7fff;
  }
  if ((param_1 & 0x4000U) != 0) {
    NHTTPi_DBGOUT_STR_("SSL: Server name does not match\n");
  }
  switch(param_1 & 0xff) {
  case 0:
    break;
  case 1:
    NHTTPi_DBGOUT_STR_("SSL: No root CA installed\n");
    break;
  case 2:
    NHTTPi_DBGOUT_STR_("SSL: Bad signature\n");
    break;
  case 3:
    NHTTPi_DBGOUT_STR_("SSL: Unknown signature algorithm\n");
    break;
  case 4:
    NHTTPi_DBGOUT_STR_("SSL: Unknown public key alrorithm\n");
  }
  if (param_1 == 0) {
    NHTTPi_DBGOUT_STR_("SSL: authorized\n");
  }
  else {
    NHTTPi_DBGOUT_STR_("SSL: not authorized\n");
  }
  return param_1;
}


void NHTTPi_SetSSLseed(void)
{
  //OS_GetLowEntropyData(0x10000);
  //SOC_AddRandomSeed(0x10000,0x20);
  return;
}


int NHTTPi_SocCancel(int param_1,int param_2)
{
  if (-1 < param_2) {
    param_1 = SOC_Shutdown(param_2,2);
  }
  return param_1;
}


int NHTTPi_SocClose(NHTTPReq *param_1, int param_2)
{
  int iVar1;
  
  iVar1 = SOC_Close(param_2);
  if (iVar1 < 0) {
    //NHTTPi_DBGOUT_ARG("NHTTPi_SocClose: socket %08x cannot be closed\n",param_2);
  } else {
    //NHTTPi_DBGOUT_ARG("NHTTPi_SocClose: socket %08x is closing in progress\n",param_2);
  }
  return iVar1;
}


int NHTTPi_SocCloseWait(NHTTPReq *param_1,int param_2)
{
  int iVar1;
  
  iVar1 = SOC_Close(param_2);
  while (iVar1 == -0x1a) {
    NHTTPi_DBGOUT_STR_("NHTTPi_SocCloseWait: waiting closed...\n");
    OS_Sleep(500);
    iVar1 = SOC_Close(param_2);
  }
  if (iVar1 < 0) {
    //NHTTPi_DBGOUT_ARG("NHTTPi_SocCloseWait: socket %08x close error(%d)\n",param_2,iVar1);
  } else {
    //NHTTPi_DBGOUT_ARG("NHTTPi_SocCloseWait: socket %08x is closed\n",param_2);
  }
  return iVar1;
}


int NHTTPi_SocConnect(NHTTPReq *req, int s, int ipAddr, int port)
{
  int iVar1;
  u8 local_28;
  u8 local_27;
  ushort local_26;
  int local_24;

  SOCSockAddrIn sockAddr;

  sockAddr.len = 8;
  sockAddr.family = 2;
  sockAddr.port = (ushort)((uint)port >> 8) & 0xff | (ushort)((port & 0xffU) << 8);
  sockAddr.addr.addr = ipAddr;
  
  //local_28 = 8;
  //local_27 = 2;
  //local_26 = (ushort)((uint)port >> 8) & 0xff | (ushort)((port & 0xffU) << 8);
  //local_24 = ipAddr;
  //iVar1 = SOC_Connect(s,&local_28);
  iVar1 = SOC_Connect(s,&sockAddr);
  if (iVar1 < 0) {
    if (req->doCancel == 0) {
      iVar1 = -0x3e9;
    }
    else {
      iVar1 = -0x3ea;
    }
  }
  else {
    iVar1 = 0;
  }
  return iVar1;
}


int NHTTPi_SocOpen(NHTTPReq *req)
{
  int iVar1;
  int iVar2;
  
  iVar1 = SOC_Socket(2,1,0);
  #ifndef SDK_PORT
  if (iVar1 < 0) 
  #else
  if (FALSE)
  #endif
  {
    NHTTPi_DBGOUT_STR_("NHTTPi_SocOpen: cannot create socket\n");
  } else {
    //NHTTPi_DBGOUT_ARG("NHTTPi_SocOpen: socket %08x opened\n",iVar1);
    if (req->isSSL != 0) {
      NHTTPi_DBGOUT_STR_("NHTTPi_SocOpen: going SSL...\n");
      NHTTPi_SetSSLseed();
      req->sslConn->ca_info = req->cainfo;
      req->sslConn->ca_builtins = req->n_ca;
      req->sslConn->auth_callback = SslAuthCallback;
      iVar2 = SOC_EnableSsl(iVar1,req->sslConn);
      if (iVar2 < 0) {
        NHTTPi_DBGOUT_STR_("NHTTPi_SocOpen: cannot enable SSL\n");
        SOC_Close(iVar1);
        iVar1 = -1;
      }
    }
  }
  return iVar1;
}


int NHTTPi_SocRecv(NHTTPReq *req, int s, void *buf, int len, int flags)
{
  int iVar1;
  
  iVar1 = SOC_Recv(s,buf,len,flags);
  if (iVar1 < 0) {
    if (req->doCancel == 0) {
      if (iVar1 == -0x38) {
        iVar1 = 0;
      }
      else {
        iVar1 = -0x3e9;
      }
    }
    else {
      iVar1 = -0x3ea;
    }
  }
  return iVar1;
}


int NHTTPi_SocSend(NHTTPReq *req, int s, void *buf, int len, int flags)
{
  int iVar1;
  
  iVar1 = SOC_Send(s,buf,len,flags);
  if (iVar1 < 0) {
    if (req->doCancel == 0) {
      if (iVar1 == -0x38) {
        iVar1 = 0;
      }
      else {
        iVar1 = -0x3e9;
      }
    }
    else {
      iVar1 = -0x3ea;
    }
  }
  return iVar1;
}