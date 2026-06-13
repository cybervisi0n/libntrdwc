#ifndef DWC_DECOMP_DEFS_H
#define DWC_DECOMP_DEFS_H
#include <nitro/types.h>
#include <nitroWiFi.h>

#include <auth/dwc_netcheck.h>
#include "base/dwc_report.h"
#include "nhttp/NHTTP.h"
#include "nd/DWCi_Nd.h"

/*---------------------------------------------------------------------------*
    Definition
 *---------------------------------------------------------------------------*/
#define ALLOC_DWC_NETCHECK 0
#define ALLOC_DWC_HTTP 1

#define DWC_HTTP_POST_FORMAT			"POST /%s HTTP/1.0\r\nContent-type: application/x-www-form-urlencoded\r\n"
#define DWC_HTTP_GET_FORMAT				"GET /%s HTTP/1.0\r\n"
#define DWC_HTTP_HEADERENTRY_FORMAT		"%s: %s\r\n"
#define DWC_HTTP_POSTITEM_FORMAT_HEAD	"%s="
#define DWC_HTTP_POSTITEM_FORMAT_MIDDLE	"&%s="

/*---------------------------------------------------------------------------*
    Type definition
 *---------------------------------------------------------------------------*/
typedef unsigned char    byte;
typedef unsigned int    dword;
typedef long long    longlong;
typedef unsigned long long    qword;
typedef int    sdword;
typedef long long    sqword;
typedef unsigned char    uchar;
typedef unsigned int    uint;
typedef unsigned long    ulong;
typedef unsigned long long    ulonglong;
typedef unsigned char    undefined1;
typedef unsigned short    undefined2;
typedef unsigned int    undefined4;
typedef unsigned long long    undefined6;
typedef unsigned long long    undefined8;
typedef unsigned short    ushort;
typedef unsigned short    word;
typedef unsigned short unsigned_short;
typedef unsigned long long unsigned_long_long;

typedef int BOOL;

typedef ushort wctype_t;

typedef struct tagACHOLD tagACHOLD, *PtagACHOLD;

struct tagACHOLD {
    void * (*alloc)(unsigned long, long);
    void (*free)(unsigned long, void *, long);
    uchar allocState;
    uchar phase;
    uchar phaseError;
    uchar findAP;
    int error;
    int errorTest;
    uchar endState;
    uchar endType;
    uchar endSequence;
    uchar type;
    char apSpotInfo[10];
    uchar overrideType;
};

typedef struct tagDWCMemPage tagDWCMemPage, *PtagDWCMemPage;

typedef struct tagDWCApInfo tagDWCApInfo, *PtagDWCApInfo;

struct tagDWCApInfo {       /* sz   offst */
    uchar ispId[32];        /* 32   0   */
    uchar ispPass[32];      /* 32   32  */
    uchar ssid[2][32];      /* 64   64  */
    uchar wep[4][16];       /* 64   128 */
    uchar ip[4];            /* 4    192 */
    uchar gateway[4];       /* 4    196 */
    uchar dns[2][4];        /* 8    200 */
    uchar netmask;          /* 1    208 */
    uchar wep2[4][5];       /* 20   209 */
    uchar authType;         /* 1    229 */
    uchar wepMode:2;        /* 1    230 */
    uchar wepNotation:6;
    uchar setType;          /* 1    231 */
    uchar rsv[7];           /* 7    232 */
    uchar state;            /* 1    239 */
};

struct tagDWCMemPage {
    struct tagDWCApInfo ap;
    uchar wifi[14];
    unsigned short crc;
};

typedef struct tagACHOLD ACHOLD;

typedef struct tagACFL tagACFL, *PtagACFL;

struct tagACFL {
    uchar state;
    uchar type;
    uchar rssi;
    uchar channel:7;
    uchar find:1;
};

typedef struct tagDWCMemPage DWCMemPage;

typedef struct tagACSL tagACSL, *PtagACSL;

typedef struct tagACSL ACSL;

struct tagACSL {        /* sz   offst*/
    uchar find:4;       /* 1    0    */
    uchar duplicate:4;
    uchar type;         /* 1    1    */
    uchar channel;      /* 1    2    */
    uchar length;       /* 1    3    */
    uchar ssid[32];     /* 1    4    */
};

typedef struct tagACWORK tagACWORK, *PtagACWORK;

typedef struct tagACWORK ACWORK;

struct tagACWORK {                      /* sz   offst*/
    struct tagDWCMemPage userInfo[3];   /* 768  0    */
    struct tagACSL searchList[9];       /* 324	768  */
    struct tagACFL findList[11];        /* 44	1092 */
    struct WMBssDesc bssDesc[11];       /* 2112	1136 */
    unsigned long long timeOut;         /* 8	3248 */
    struct WCMWepDesc wepKey;           /* 82	3256 */
    uchar dmaNo;                        /* 1	3338 */
    uchar powerMode:2;                  /* 1	3339 */
    uchar authMode:2;
    uchar aroundCount:4;
    uchar connectType:4;                /* 1    3340 */
    uchar skipNetCheck:2;
    uchar duplicateFlag:2;
    uchar connectApType;                /* 1    3341 */
    uchar phaseBak;                     /* 1    3342 */
    uchar searchListNo;                 /* 1    3343 */
    uchar searchListNum;                /* 1    3344 */
    char searchChannel;                 /* 1    3345 */
    uchar findListNum;                  /* 1    3346 */
    uchar connectNo;                    /* 1    3347 */
    uchar connectResult;                /* 1    3348 */
    uchar count;                        /* 1    3349 */
    unsigned short stealthChannel;      /* 2    3350 */
};

typedef struct tagDWCApInfo DWCBmApInfo;

typedef struct tagACFL ACFL;

typedef struct tagDWCACOption DWCACOption;


//dwc_netcheck.c
typedef struct tagDWCWiFiInfo tagDWCWiFiInfo, *PtagDWCWiFiInfo;

typedef struct tagDWCWiFiInfo DWCWiFiInfo;

struct tagDWCWiFiInfo {
    unsigned long long attestedUserId;
    unsigned long long notAttestedId;
    unsigned short pass;
    unsigned short randomHistory;
};

typedef struct tagMEMWRITEC tagMEMWRITEC, *PtagMEMWRITEC;

struct tagMEMWRITEC {
    uchar work[256];
    struct MATHCRC16Table tbl;
};

typedef struct tagMEMINIT tagMEMINIT, *PtagMEMINIT;

typedef struct tagDWCMemMap tagDWCMemMap, *PtagDWCMemMap;

struct tagDWCMemMap {
    struct tagDWCMemPage page[4];
};

struct tagMEMINIT {
    struct tagDWCMemMap mm;
    uchar work[256];
    struct MATHCRC16Table tbl;
};

typedef struct DWCCalInfo DWCCalInfo, *PDWCCalInfo;

struct DWCCalInfo {
    char userid[14];
    char passwd[7];
    char gamecd[5];
    char makercd[3];
    char unitcd[2];
    char macadr[13];
    char lang[3];
    char birth[5];
    char devtime[13];
    char bssid[13];
    char ssid[33];
    char apinfo[14];
    unsigned short devname[11];
};

typedef struct DWCSimpleNetcheck DWCSimpleNetcheck, *PDWCSimpleNetcheck;

struct DWCSimpleNetcheck {
    int sock;
    struct SOCSockAddrIn socaddr;
    struct SOCPollFD pollfd;
    char recvbuf[32];
    int recvindex;
    int stage;
    int result;
    unsigned long long finishtick;
};

typedef struct DWCNetcheck DWCNetcheck, *PDWCNetcheck;

struct DWCNetcheck {
    uchar stack[4096];
    int error;
    int returncode;
    DWCHttpLabelValue http_labelvalue[32];
    DWCNetcheckParam param;
    char *body_302;
    char *body_wayport;
    struct _OSThread thread;
    struct OSMutex mutex;
};

typedef struct
{
	int	retry;
	DWCAuthError error;
	DWCAuthResult 	result;
	DWCAuthParam	param;
	DWCHttpLabelValue http_labelvalue[32];
	DWCHttp *http;
	OSThread		thread;
	OSMutex			mutex;
	BOOL			abort;
	unsigned char	stack[4096] ATTRIBUTE_ALIGN(32);
} DWCAuth;

typedef struct tagNHTTPi_DATALIST tagNHTTPi_DATALIST, *PtagNHTTPi_DATALIST;

struct tagNHTTPi_DATALIST {
    struct tagNHTTPi_DATALIST *prev;
    struct tagNHTTPi_DATALIST *next;
    char *label;
    char *value;
    unsigned long length;
    int isBinary;
};

typedef struct _NHTTPRes _NHTTPRes, *P_NHTTPRes;

typedef struct tagNHTTPi_HDRBUFLIST tagNHTTPi_HDRBUFLIST, *PtagNHTTPi_HDRBUFLIST;

struct _NHTTPRes {
    int nLenHeader;
    int nLenBody;
    int isSuccess;
    unsigned long nLenRecvBuf;
    char *allHeader;
    char *foundHeader;
    char *pRecvBuf;
    struct tagNHTTPi_HDRBUFLIST *pHdrBufBlock;
    char pHdrBufFirst[1024];
};

struct tagNHTTPi_HDRBUFLIST {
    struct tagNHTTPi_HDRBUFLIST *next;
    char block[512];
};

typedef struct tagNHTTPi_REQLIST tagNHTTPi_REQLIST, *PtagNHTTPi_REQLIST;

typedef struct tagNHTTPi_REQLIST NHTTPi_REQLIST;

typedef struct _NHTTPReq _NHTTPReq, *P_NHTTPReq;

struct tagNHTTPi_REQLIST {
    struct tagNHTTPi_REQLIST *prev;
    struct tagNHTTPi_REQLIST *next;
    int id;
    struct _NHTTPReq *req;
    void *socket;
};

struct _NHTTPReq {
    int isStarted;
    int doCancel;
    int isSSL;
    int nLenHost;
    int nLenHostDesc;
    int nContentLength;
    int n_ca;
    SOCLCaInfo **cainfo;
    CPSSslConnection *sslConn;
    char *pURL;
    int port;
    void *param;
    struct _NHTTPRes *response;
    struct tagNHTTPi_DATALIST *pListHeader;
    struct tagNHTTPi_DATALIST *pListPost;
    NHTTPReqCallback pCallback;
    NHTTPReqMethod method;
    char tagPost[20];
};

struct anon_struct_4_1_6b219b52_for_count {
    int *entrynum;
};

struct anon_struct_20_5_2919e7c3_for_list {
    DWCNdFileInfo *filelist;
    int intnum;
    char *recvbuf;
    char *offset;
    char *num;
};

struct anon_struct_8_2_55371a73_for_file {
    char *filename;
    uint filesize;
};

union anon_union_20_3_ede8f6f1_for_param {
    struct anon_struct_4_1_6b219b52_for_count count;
    struct anon_struct_20_5_2919e7c3_for_list list;
    struct anon_struct_8_2_55371a73_for_file file;
};

struct tagDWCi_Ndi_HTTPREQ {
    int id;
    DWCNdCallbackReason reason;
    DWCNdCallback callback;
    void *threadEvent;
    union anon_union_20_3_ede8f6f1_for_param param;
};

typedef struct tagDWCi_Ndi_HTTPREQ tagDWCi_Ndi_HTTPREQ, *PtagDWCi_Ndi_HTTPREQ;

/*---------------------------------------------------------------------------*
    Function definition
 *---------------------------------------------------------------------------*/

//--NitroWifi External
extern u8*     WCM_GetApEssid(u16* length);

//--libdwcac
//beacon.c

//callback.c
void DWCi_AC_CallBackWCM(WCMNotice *param_1);

//close.c

//connect.c

//dwc_ac.c
void  DWCi_AC_Free          ( u32 name, void *ptr, s32 size );
void DWCi_AC_FreeAll(void);

int DWCi_AC_GetError(void);
void * DWCi_AC_GetMemPtr(u32 param_1);
uchar DWCi_AC_GetPhase(void);

//error.c
u8 DWCi_AC_Error(void);
int DWCi_AC_GetResult(void);

//makelist.c
/*undefined*/uchar DWCi_AC_MakeSearchList(int aSearchListType);
enum {
    DWCi_AC_MakeSearchList_Around,
    DWCi_AC_MakeSearchList_DifferChannel,
    DWCi_AC_MakeSearchList_Stealth
};

//retry.c
byte DWCi_AC_ConnectRetryAP(void);

//search.c

//shop_usb.c

//test.c


//--libdwcauth
//dwc_auth.c
DWCAuthError DWCi_Auth_FillResult(void);
DWCAuthError DWCi_Auth_ParseHttp(void);
DWCAuthError DWCi_Auth_Prepare_FirstPost(DWCHttp *http, const char *gsbrcd, DWCAuthParam * param, BOOL parse);
undefined4 DWCi_Auth_Start(BOOL getwifiinfofromflash);
void DWCi_Auth_StartThread(void);
void DWCi_Auth_Thread(void *arg);

//dwc_cal.c
void DWC_Auth_GetCalInfoFromNVRAM(char * aBuf);
undefined4 DWC_Auth_GetCalInfoFromWiFiInfo(char * param_1, DWCWiFiInfo *aWifiInfo);
undefined4 DWC_Auth_SetCalInfoToHttp(DWCHttp *aHttp,char *param_2,int param_3);
void DWC_Auth_PrintCalInfo(char * param_1);

//dwc_http.c
BOOL DWCi_Http_Yield(DWCHttp * aHttp);
void DWCi_Http_InitCpsSocket(DWCHttp * aHttp);
undefined4 DWCi_Http_CheckHeaderReceived(DWCHttp * aHttp);
void DWCi_Http_Thread(void * arg);
BOOL DWCi_Http_WriteBasicHeader(DWCHttp * aHttp);
uint DWCi_Http_CPSCallback(uint param_1);
undefined4 DWCi_Http_AllocBuffer(DWCHttp * aHttp,DWCHttpBuffer * aBuf,int aLen);
void DWCi_Http_FreeBuffer(DWCHttp* aHttp , DWCHttpBuffer * aBuf);
undefined4 DWCi_Http_ReallocBuffer(DWCHttp * aHttp,DWCHttpBuffer *aBuffer,int aLen);
undefined4 DWCi_Http_SetHostinfo(DWCHttp *aHttp, char * aUrl);
int DWCi_Http_AddResult(DWCHttp * aHttp, DWCHttpParseResult *aResult, char *aLabel, char *aContent);
void DWCi_Http_DestroyResult(int param_1,int param_2,int param_3,undefined4 param_4);
CPSInAddr DWCi_Http_Resolve(DWCHttp * aHttp);

//dwc_netcheck.c
void DWCi_Netcheck_SetError(DWCNetcheckError aErr);
void DWCi_Netcheck_StartThread(void);
void DWCi_Netcheck_Thread(void * arg);
void DWCi_SimpleNetcheckSendRequest(void);
void DWCi_SimpleNetcheckReceiveReply(void);

//dwc_svl.c
int DWCi_Svl_ParseHttp(void);


//--libdwcbm
//dwc_backup.c
BOOL DWCi_BM_GetApInfo(DWCMemPage* buf);
void  DWCi_BM_GetWiFiInfo(DWCWiFiInfo* buf);
BOOL DWCi_BM_SetWiFiInfo(undefined4 param_1,/*EVP_PKEY_CTX*/ void *param_2);
BOOL DWCi_BACKUPlInit(ushort *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4);
BOOL DWCi_BACKUPlRead(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4);
BOOL DWCi_BACKUPlWritePage(int param_1,int param_2,uchar *param_3,size_t param_4);
BOOL DWCi_BACKUPlWriteAll(uint param_1,undefined4 param_2,undefined4 param_3,size_t param_4);
undefined4 DWCi_BACKUPlWriteChild(int param_1,uchar *param_2,undefined4 param_3,size_t param_4);
void DWCi_BACKUPlSetWiFi(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4);
char DWCi_BACKUPlConvMaskCidr(int param_1);
void DWCi_BACKUPlConvMaskAddr(uint param_1,int param_2);
undefined4 DWC_BACKUPlCheckSsid(int param_1);
undefined4 DWC_BACKUPlCheckIp(undefined4 param_1,undefined4 param_2);
undefined4 DWC_BACKUPlCheckAddress(byte *param_1);
uchar * DWCi_BACKUPlConvWifiInfo(int param_1);
uchar * DWCi_BACKUPlGetWifi(void);


//--libdwcnd
//dwci_nd_attr.c
void DWCi_Ndi_freeAttr(void);

//dwci_nd_base64.c
int DWCi_Ndi_decodeBASE64(char *param_1, char *param_2, int param_3);
int DWCi_Ndi_encodeBASE64(char *param_1,char *param_2,int param_3);
int DWCi_Ndi_getDecodeLenBASE64(int param_1);
int DWCi_Ndi_getEncodeLenBASE64(int param_1);

//dwci_nd_dwc_nitr.c
void DWCi_Ndi_getUserAgent(char *aUserAgentBuf);

//dwci_nd_filecount.c
BOOL DWCi_Ndi_response_count(NHTTPRes *res, tagDWCi_Ndi_HTTPREQ *httpReq);

//dwci_nd_fileget.c
BOOL DWCi_Ndi_response_contents(NHTTPRes *res, tagDWCi_Ndi_HTTPREQ *httpReq);

//dwci_nd_filelist.c
BOOL DWCi_Ndi_response_list(NHTTPRes *res, tagDWCi_Ndi_HTTPREQ *httpReq);

//dwci_nd_os_nitro.c
void DWCi_Ndi_destroyThreadEvent(void * param_1);
void DWCi_Ndi_exitHttpCallback(void);
void DWCi_Ndi_exitLockGlobal(void);
void DWCi_Ndi_getMacAddress(char *aMacAddr);
BOOL DWCi_Ndi_initHttpCallback(void);
BOOL DWCi_Ndi_initLockGlobal(void);
void DWCi_Ndi_kickThread(OSMessageQueue *param_1);
void DWCi_Ndi_lockGlobal(void);
BOOL DWCi_Ndi_prepareHttpCallback(void);
void DWCi_Ndi_unlockGlobal(void);
void DWCi_Ndi_unprepareHttpCallback(void);

//dwci_nd_stdlib_n.c
void DWCi_Ndi_memclr(void *dest, u32 size);
void DWCi_Ndi_memcpy(void *dst, void *src, int len);
int DWCi_Ndi_strcmp(char *str1,char *str2);
int DWCi_Ndi_strncmp(char *str1,char *str2,int len);
int DWCi_Ndi_strlen(char *param_1);

//dwci_nd_string.c
int DWCi_Ndi_intToStr(char *str, int num);
int DWCi_Ndi_strToInt(char *str, int len);


//--libdwcnhttp
//nhttp_bgnend.c
void NHTTPi_CleanupThreadProc(void * callback);

//nhttp_control.c
tagNHTTPi_DATALIST * NHTTPi_getHdrFromList(tagNHTTPi_DATALIST **param_1);

//nhttp_list.c
void NHTTPi_allFreeReqQueue(void);
tagNHTTPi_REQLIST * NHTTPi_findReqQueue(int id);
int NHTTPi_freeReqQueue(int id);
tagNHTTPi_REQLIST * NHTTPi_getReqFromReqQueue(void);
int NHTTPi_setReqQueue(NHTTPReq *param_1);

//nhttp_os_nitro.c
BOOL NHTTPi_createCommThread(u32 prio, void * stack);
void NHTTPi_DBGOUT_STR_(char *aString);
void NHTTPi_destroyCommThread(void);
void NHTTPi_exitLockReqList(void);
void NHTTPi_idleCommThread(void);
BOOL NHTTPi_initLockReqList(void);
void NHTTPi_kickCommThread(void);
void NHTTPi_lockReqList(void);
void NHTTPi_unlockReqList(void);

//nhttp_recvbuf.c
int NHTTPi_compareTokenN_HdrRecvBuf(NHTTPRes *res,int param_2,int param_3,char *param_4,char param_5);
int NHTTPi_findNextLineHdrRecvBuf(NHTTPRes *res, int param_2, int param_3, int *param_4);
BOOL NHTTPi_isRecvBufFull(NHTTPRes *res,uint len);
int NHTTPi_loadFromHdrRecvBuf(NHTTPRes *res, char *param_2, int param_3, int param_4);
int NHTTPi_RecvBuf(NHTTPReq *req,int socket,int offset,int recvFlags);
int NHTTPi_RecvBufN(NHTTPReq *req,int socket,int offset,int len,int recvFlags);
int NHTTPi_skipSpaceHdrRecvBuf(NHTTPRes *param_1, int param_2, int param_3);

//nhttp_request.c
void NHTTPi_cancelAllRequests(void);
int NHTTPi_destroyRequestObject(NHTTPReq *req);

//nhttp_resolve_nitro.c
u32 NHTTPi_resolveHostname(NHTTPReq *req);

//nhttp_response.c
int NHTTPi_getHeaderValue(NHTTPRes *res, char *label, int *param_3);

//nhttp_socket_nitro.c
void NHTTPi_SetSSLseed(void);
int NHTTPi_SocCancel(int param_1,int param_2);
int NHTTPi_SocClose(NHTTPReq *param_1, int param_2);
int NHTTPi_SocCloseWait(NHTTPReq *param_1, int param_2);
int NHTTPi_SocConnect(NHTTPReq *req, int s, int ipAddr, int port);
int NHTTPi_SocOpen(NHTTPReq *req);
int NHTTPi_SocRecv(NHTTPReq *req, int s, void *buf, int len, int flags);
int NHTTPi_SocSend(NHTTPReq *req, int s, void *buf, int len, int flags);

//nhttp_stdlib_nitro.c
void NHTTPi_memclr(void *dest, u32 size);
void NHTTPi_memcpy(void *dst, void *src, int len);
int NHTTPi_strcmp(char *str1, char *str2);
int NHTTPi_strncmp(char *str1, char *str2, int len);
int NHTTPi_strlen(char *param_1);
int NHTTPi_stricmp(char *string1, char *string2);
int NHTTPi_strnicmp(char *string1, char *string2, int len);

//nhttp_thread.c
void NHTTPi_CommThreadProc(void * arg);

//nhttp_token.c
int NHTTPi_compareToken(char *param_1, char *param_2);
int NHTTPi_encodeUrlChar(char *param_1, char param_2);
int NHTTPi_getUrlEncodedSize(char *param_1);
int NHTTPi_intToStr(char *str, int num);
int NHTTPi_memfind(char *param_1, int param_2, char *param_3, int param_4);
int NHTTPi_strToHex(char *param_1, int param_2);
int NHTTPi_strToInt(char *str, int len);
int NHTTPi_strtonum(char *param_1, int param_2);

#endif
