#include <nitro.h>
#include <dwc.h>
#include <nd/DWCi_Nd.h>

#include "decomp/decomp_defs.h"
#include "nhttp/NHTTP.h"

extern BOOL DWCi_Ndi_isOpened;
extern int DWCi_Ndi_isExistApinfo;
extern int DWCi_Ndi_nhttpid;
extern DWCNdError DWCi_Ndi_error;
extern DWCNdAlloc DWCi_Ndi_alloc;
extern DWCNdFree DWCi_Ndi_free;
extern char * DWCi_Ndi_serverURL;
extern char * DWCi_Ndi_dlattr[3];
extern char * DWCi_Ndi_authtoken;
extern char DWCi_Ndi_password[25];
extern char DWCi_Ndi_dwcgamecd[9];
extern char DWCi_Ndi_userAgent[33];
extern char DWCi_Ndi_macaddr[17];
extern char DWCi_Ndi_strApinfo[21];
extern DWCNdCallback DWCi_Ndi_dlcallback;


extern CPSCaInfo dwci_nd_ca_nasself;

static CPSCaInfo * ca_array[] = {&dwci_nd_ca_nasself};

static char rhgamecode[9];
static char listnumRecvbuf[128];
static char userid[21];

static void freeHttp(tagDWCi_Ndi_HTTPREQ *httpReq, NHTTPReq *req);
static void req_callback(NHTTPError error, NHTTPRes *res, void * param);
static BOOL runHttp(tagDWCi_Ndi_HTTPREQ *httpReq, NHTTPReq *req);
static int setupHttp(tagDWCi_Ndi_HTTPREQ **param_1, NHTTPReq **param_2, char *postAction, char *buf, u32 len);


static void freeHttp(tagDWCi_Ndi_HTTPREQ *httpReq,NHTTPReq *req)
{
  NHTTP_DestroyRequest(req);
  DWCi_Ndi_unprepareHttpCallback();
  (*DWCi_Ndi_free)(httpReq);
  return;
}


static void req_callback(NHTTPError error, NHTTPRes *res, void * param)
{
  tagDWCi_Ndi_HTTPREQ * arg = (tagDWCi_Ndi_HTTPREQ *)param;

  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  DWCNdError ndError;
  BOOL bVar6;
  u8 auStack_4c [4];
  char *local_48;
  char *local_44;
  int local_40;
  void *local_3c;
  DWCNdCallback callback;
  DWCNdCallbackReason cbReason;
  
  cbReason = arg->reason;
  callback = arg->callback;
  local_3c = arg->threadEvent;
  local_40 = -1;
  if (error == 0) {
    iVar1 = NHTTP_GetHeaderField(res,"HTTPSTATUSCODE",&local_44);
    if (iVar1 == 3) {
      iVar1 = DWCi_Ndi_strToInt(local_44,3);
      if (iVar1 == 302) {
        bVar6 = FALSE;
        iVar1 = NHTTP_GetBodyAll(res,&local_48);
        if (iVar1 < 0) {
          ndError = DWC_ND_ERROR_HTTP;
        } else {
          iVar2 = 0;
          do {
            iVar3 = iVar2;
            if (iVar1 <= iVar2) goto LAB_000102b0;
            for (; (local_48[iVar3] != '=' && (iVar3 < iVar1)); iVar3 = iVar3 + 1) {
            }
            iVar4 = iVar3 + 1;
            iVar5 = iVar4;
            if (iVar3 - iVar2 == 8) {
              iVar2 = DWCi_Ndi_strncmp(local_48 + iVar2,"returncd",8);
              bVar6 = iVar2 == 0;
            }
            for (; (((local_48[iVar5] != '&' && (local_48[iVar5] != '\r')) &&
                    (local_48[iVar5] != '\0')) && (iVar5 < iVar1)); iVar5 = iVar5 + 1) {
            }
            iVar2 = iVar5 + 1;
          } while (!bVar6);
          if (iVar5 - iVar4 == 4) {
            iVar1 = DWCi_Ndi_decodeBASE64(auStack_4c,local_48 + iVar4,4);
            auStack_4c[iVar1] = 0;
            local_40 = DWCi_Ndi_strToInt(auStack_4c,iVar1);
          }
LAB_000102b0:
          if (local_40 < 0) {
            ndError = DWC_ND_ERROR_HTTP;
          }
          else {
            ndError = DWC_ND_ERROR_DLSERVER;
          }
        }
      } else if ((iVar1 < 1) || (iVar1 != 200)) {
        ndError = DWC_ND_ERROR_HTTP;
      } else if (cbReason == DWC_ND_CBREASON_GETFILELISTNUM) {
        iVar1 = DWCi_Ndi_response_count(res, arg);
        if (iVar1 == 0) {
          ndError = DWC_ND_ERROR_HTTP;
        }
        else {
          ndError = DWC_ND_ERROR_NONE;
        }
      } else if (cbReason == DWC_ND_CBREASON_GETFILELIST) {
        iVar1 = DWCi_Ndi_response_list(res,arg);
        if (iVar1 == 0) {
          ndError = DWC_ND_ERROR_HTTP;
        }
        else {
          ndError = DWC_ND_ERROR_NONE;
        }
      } else if (cbReason == DWC_ND_CBREASON_GETFILE) {
        iVar1 = DWCi_Ndi_response_contents(res,arg);
        if (iVar1 == 0) {
          ndError = DWC_ND_ERROR_HTTP;
        }
        else {
          ndError = DWC_ND_ERROR_NONE;
        }
      }
    } else {
      ndError = DWC_ND_ERROR_HTTP;
    }
  } else if (error == 6) {
    ndError = DWC_ND_ERROR_BUFFULL;
  } else if (error == 8) {
    ndError = DWC_ND_ERROR_CANCELED;
  } else {
    ndError = DWC_ND_ERROR_HTTP;
  }
  if (cbReason != DWC_ND_CBREASON_GETFILELISTNUM) {
    if (cbReason == DWC_ND_CBREASON_GETFILELIST) {
      if ((arg->param).list.num != (char *)0x0) {
        (*DWCi_Ndi_free)((arg->param).list.num);
      }
      if ((arg->param).list.offset != (char *)0x0) {
        (*DWCi_Ndi_free)((arg->param).list.offset);
      }
      (*DWCi_Ndi_free)((arg->param).list.recvbuf);
    }
    else if (cbReason == DWC_ND_CBREASON_GETFILE) {
      (*DWCi_Ndi_free)((arg->param).list.filelist);
    }
  }
  DWCi_Ndi_lockGlobal();
  DWCi_Ndi_nhttpid = 0xffffffff;
  DWCi_Ndi_unlockGlobal();
  NHTTP_DestroyResponse(res);
  (*DWCi_Ndi_free)(arg);
  DWCi_Ndi_unprepareHttpCallback();
  if (ndError != DWC_ND_ERROR_NONE) {
    DWCi_Ndi_error = ndError;
  }
  if (callback != NULL) {
    (*callback)(cbReason,ndError,local_40);
  }
  if (local_3c != (void *)0x0) {
    DWCi_Ndi_kickThread(local_3c);
  }
  return;
}


static BOOL runHttp(tagDWCi_Ndi_HTTPREQ *httpReq,NHTTPReq *req)
{
  int iVar1;
  BOOL bVar2;
  
  DWCi_Ndi_lockGlobal();
  //dbg_req = req;
  iVar1 = NHTTP_SendRequestAsync(req);
  httpReq->id = iVar1;
  DWCi_Ndi_nhttpid = httpReq->id;
  DWCi_Ndi_unlockGlobal();
  bVar2 = -1 < httpReq->id;
  if (!bVar2) {
    freeHttp(httpReq,req);
    DWCi_Ndi_error = 3;
  }
  return bVar2;
}

static int setupHttp(tagDWCi_Ndi_HTTPREQ **param_1,NHTTPReq **param_2,char *postAction,char *buf,u32 len)
{
  int iVar1;
  tagDWCi_Ndi_HTTPREQ *httpReq;
  NHTTPReq *req;
  BOOL BVar2;

  BVar2 = 1;
  
  //iVar1 = DWCi_Ndi_getDSdesc(rhgamecode,userid);
  if (iVar1 == 0) {
    DWCi_Ndi_error = 8;
    iVar1 = 0;
  } else {
    httpReq = (tagDWCi_Ndi_HTTPREQ *)(*DWCi_Ndi_alloc)(sizeof(tagDWCi_Ndi_HTTPREQ),4);
    if (httpReq == NULL) {
      DWCi_Ndi_error = 1;
      iVar1 = 0;
    } else {
      httpReq->callback = DWCi_Ndi_dlcallback;
      httpReq->threadEvent = (void *)0x0;
      iVar1 = DWCi_Ndi_prepareHttpCallback();
      if (iVar1 == 0) {
        (*DWCi_Ndi_free)(httpReq);
        DWCi_Ndi_error = 2;
        iVar1 = 0;
      } else {
        req = NHTTP_CreateRequest(DWCi_Ndi_serverURL,1,buf,len,req_callback,httpReq);
        if (req == (NHTTPReq *)0x0) {
          DWCi_Ndi_unprepareHttpCallback();
          (*DWCi_Ndi_free)(httpReq);
          DWCi_Ndi_error = 3;
          iVar1 = 0;
        } else {
          *param_1 = httpReq;
          *param_2 = req;
          iVar1 = NHTTP_SetCAChain(req, ca_array, 1);
          if ((((((((BVar2 == 0) ||
                   (BVar2 = NHTTP_AddHeaderField(req, "User-Agent",DWCi_Ndi_userAgent), BVar2 == 0)) ||
                  (BVar2 = NHTTP_AddPostDataAscii(req, "gamecd",DWCi_Ndi_dwcgamecd), BVar2 == 0)) ||
                 ((BVar2 = NHTTP_AddPostDataAscii(req, "rhgamecd",rhgamecode), BVar2 == 0 ||
                  (BVar2 = NHTTP_AddPostDataAscii(req, "passwd",DWCi_Ndi_password), BVar2 == 0)))) ||
                ((BVar2 = NHTTP_AddPostDataAscii(req, "token",DWCi_Ndi_authtoken), BVar2 == 0 ||
                 ((BVar2 = NHTTP_AddPostDataAscii(req, "userid",userid), BVar2 == 0 ||
                  (BVar2 = NHTTP_AddPostDataAscii(req, "macadr",DWCi_Ndi_macaddr), BVar2 == 0)))))) ||
               (BVar2 = NHTTP_AddPostDataAscii(req, "action",postAction), BVar2 == 0)) ||
              (((DWCi_Ndi_dlattr != (char *)0x0 &&
                (BVar2 = NHTTP_AddPostDataAscii(req, "attr1",DWCi_Ndi_dlattr[0]), BVar2 == 0)) ||
               ((DWCi_Ndi_isExistApinfo != (char *)0x0 &&
                (BVar2 = NHTTP_AddPostDataAscii(req, "attr2",(char *)&DWCi_Ndi_isExistApinfo), BVar2 == 0))))))
             || (((DWCi_Ndi_strApinfo != (char *)0x0 &&
                  (BVar2 = NHTTP_AddPostDataAscii(req, "attr3",DWCi_Ndi_strApinfo), BVar2 == 0)) ||
                 ((DWCi_Ndi_isExistApinfo != (char *)0x0 &&
                  (BVar2 = NHTTP_AddPostDataAscii(req, "apinfo",DWCi_Ndi_strApinfo),
                  BVar2 == 0)))))) {
            freeHttp(httpReq,req);
            DWCi_Ndi_error = 1;
            iVar1 = 0;
          }
          else {
            iVar1 = 1;
          }
        }
      }
    }
  }
  return iVar1;
}

BOOL DWCi_NdGetFileAsync( DWCNdFileInfo* filelist, char* buf, unsigned int bufsize )
{
  BOOL BVar1;
  int iVar2;
  int iVar3;
  char *value;
  int iVar4;
  NHTTPReq *req;
  tagDWCi_Ndi_HTTPREQ *local_30 [2];
  
  if (DWCi_Ndi_isOpened == 0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x22b,"Failed assertion DWCi_Ndi_isOpened");
  }
  if (filelist == (DWCNdFileInfo *)0x0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x22c,"Failed assertion filelist");
  }
  if (buf == (char *)0x0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x22d,"Failed assertion buf");
  }
  if (filelist == (DWCNdFileInfo *)0x0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x22e, "Failed assertion filelist->name");
  }
  if (filelist->size == 0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x22f, "Failed assertion filelist->size");
  }
  if (bufsize < filelist->size) {
    DWCi_Ndi_error = 7;
    BVar1 = 0;
  } else {
    iVar2 = DWCi_Ndi_strlen(filelist->name);
    if (iVar2 < 0x21) {
      if (iVar2 < 1) {
        iVar3 = 0;
      }
      else {
        iVar3 = DWCi_Ndi_getEncodeLenBASE64(iVar2);
      }
      value = (char *)(*DWCi_Ndi_alloc)(iVar3 + 1,4);
      if (value == (char *)0x0) {
        DWCi_Ndi_error = 1;
        BVar1 = 0;
      }
      else {
        iVar4 = setupHttp(local_30,&req,"Y29udGVudHM*",buf,filelist->size);
        if (iVar4 == 0) {
          (*DWCi_Ndi_free)(value);
          BVar1 = 0;
        }
        else {
          if (0 < iVar3) {
            iVar2 = DWCi_Ndi_encodeBASE64(value,filelist->name,iVar2);
            value[iVar2] = '\0';
            BVar1 = NHTTP_AddPostDataAscii(req,"contents",value);
            if (BVar1 == 0) {
              freeHttp(local_30[0],req);
              (*DWCi_Ndi_free)(value);
              DWCi_Ndi_error = 1;
              return 0;
            }
          }
          local_30[0]->reason = DWC_ND_CBREASON_GETFILE;
          (local_30[0]->param).file.filename = value;
          (local_30[0]->param).list.intnum = filelist->size;
          iVar2 = runHttp(local_30[0],req);
          if (iVar2 == 0) {
            (*DWCi_Ndi_free)(value);
            BVar1 = 0;
          }
          else {
            BVar1 = 1;
          }
        }
      }
    }
    else {
      DWCi_Ndi_error = 7;
      BVar1 = 0;
    }
  }
  return BVar1;
}


BOOL DWCi_NdGetFileListAsync( DWCNdFileInfo* filelist, int offset, int num )
{
  char *buf;
  BOOL BVar1;
  undefined4 uVar2;
  int iVar3;
  char *value;
  char *value_00;
  int iVar4;
  NHTTPReq *req;
  tagDWCi_Ndi_HTTPREQ *httpReq;
  char auStack_3c [12];
  
  if (DWCi_Ndi_isOpened == 0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x1c7,"Failed assertion DWCi_Ndi_isOpened");
  }
  if (filelist == (DWCNdFileInfo *)0x0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x1c8,"Failed assertion filelist");
  }
  if (offset < 0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x1c9,"Failed assertion offset >= 0");
  }
  if (num < 1) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x1ca,"Failed assertion num > 0");
  }
  iVar4 = num * 0x100;
  if (iVar4 < 0x80) {
    iVar4 = 0x80;
  }
  buf = (char *)(*DWCi_Ndi_alloc)(iVar4,4);
  if (buf == (char *)0x0) {
    DWCi_Ndi_error = 1;
    BVar1 = 0;
  } else {
    uVar2 = DWCi_Ndi_intToStr(auStack_3c,offset);
    iVar3 = DWCi_Ndi_getEncodeLenBASE64(uVar2);
    value = (char *)(*DWCi_Ndi_alloc)(iVar3 + 1,4);
    if (value == (char *)0x0) {
      (*DWCi_Ndi_free)(buf);
      DWCi_Ndi_error = 1;
      BVar1 = 0;
    } else {
      iVar3 = DWCi_Ndi_encodeBASE64(value,auStack_3c,uVar2);
      value[iVar3] = '\0';
      uVar2 = DWCi_Ndi_intToStr(auStack_3c,num);
      iVar3 = DWCi_Ndi_getEncodeLenBASE64(uVar2);
      value_00 = (char *)(*DWCi_Ndi_alloc)(iVar3 + 1,4);
      if (value_00 == (char *)0x0) {
        (*DWCi_Ndi_free)(value);
        (*DWCi_Ndi_free)(buf);
        DWCi_Ndi_error = 1;
        BVar1 = 0;
      }
      else {
        iVar3 = DWCi_Ndi_encodeBASE64(value_00,auStack_3c,uVar2);
        value_00[iVar3] = '\0';
        iVar4 = setupHttp(&httpReq,&req,"bGlzdA**",buf,iVar4);
        if (iVar4 == 0) {
          (*DWCi_Ndi_free)(value_00);
          (*DWCi_Ndi_free)(value);
          (*DWCi_Ndi_free)(buf);
          BVar1 = 0;
        }
        else {
          BVar1 = NHTTP_AddPostDataAscii(req,"offset",value);
          if ((BVar1 == 0) ||
             (BVar1 = NHTTP_AddPostDataAscii(req,"num",value_00), BVar1 == 0)) {
            (*DWCi_Ndi_free)(value_00);
            (*DWCi_Ndi_free)(value);
            (*DWCi_Ndi_free)(buf);
            freeHttp(httpReq,req);
            DWCi_Ndi_error = 1;
            BVar1 = 0;
          }
          else {
            httpReq->reason = DWC_ND_CBREASON_GETFILELIST;
            (httpReq->param).list.filelist = filelist;
            (httpReq->param).list.offset = value;
            (httpReq->param).list.num = value_00;
            (httpReq->param).list.intnum = num;
            (httpReq->param).list.recvbuf = buf;
            iVar4 = runHttp(httpReq,req);
            if (iVar4 == 0) {
              (*DWCi_Ndi_free)(value_00);
              (*DWCi_Ndi_free)(value);
              (*DWCi_Ndi_free)(buf);
              BVar1 = 0;
            }
            else {
              BVar1 = 1;
            }
          }
        }
      }
    }
  }
  return BVar1;
}


BOOL DWCi_NdGetFileListNumAsync( int* entrynum )
{
  int iVar1;
  BOOL BVar2;
  NHTTPReq *local_14;
  tagDWCi_Ndi_HTTPREQ *local_10 [2];
  
  if (DWCi_Ndi_isOpened == 0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x19b,"Failed assertion DWCi_Ndi_isOpened");
  }
  if (entrynum == (int *)0x0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x19c,"Failed assertion entrynum");
  }
  iVar1 = setupHttp(local_10,&local_14,"Y291bnQ*",listnumRecvbuf,0x80);
  if (iVar1 == 0) {
    BVar2 = 0;
  }
  else {
    local_10[0]->reason = DWC_ND_CBREASON_GETFILELISTNUM;
    (local_10[0]->param).file.filename = (char *)entrynum;
    iVar1 = runHttp(local_10[0],local_14);
    if (iVar1 == 0) {
      BVar2 = 0;
    }
    else {
      BVar2 = 1;
    }
  }
  return BVar2;
}


BOOL DWCi_NdGetProgress( u32* received, u32* contentlen )
{
  BOOL BVar1;
  
  if (DWCi_Ndi_isOpened == 0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x274,"Failed assertion DWCi_Ndi_isOpened");
  }
  if (received == (u32 *)0x0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x275,"Failed assertion received");
  }
  if (contentlen == (u32 *)0x0) {
    OSi_Panic("DWCi_Nd_fileapi.c",0x276,"Failed assertion contentlen");
  }
  BVar1 = NHTTP_GetProgress(received,contentlen);
  return BVar1;
}