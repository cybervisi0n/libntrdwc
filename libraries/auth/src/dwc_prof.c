typedef unsigned char   undefined;

typedef unsigned char    byte;
typedef unsigned int    dword;
typedef unsigned long long    qword;
typedef int    sdword;
typedef unsigned char    uchar;
typedef unsigned int    uint;
typedef unsigned long    ulong;
typedef unsigned long long    ulonglong;
typedef unsigned char    undefined1;
typedef unsigned int    undefined4;
typedef unsigned long long    undefined5;
typedef unsigned long long    undefined6;
typedef unsigned long long    undefined8;
typedef unsigned short    ushort;
typedef int    wchar_t;
typedef unsigned short    word;
typedef pointer pointer __((offset(0x24)));

typedef pointer pointer __((offset(0x10)));

typedef pointer pointer __((offset(0xc)));

typedef pointer pointer __((offset(0x1f0)));

typedef pointer pointer __((offset(0x4d4)));

typedef pointer pointer __((offset(0x74)));

typedef pointer pointer __((offset(0x174)));

typedef pointer pointer __((offset(0x20)));

typedef pointer pointer __((offset(0x1ec)));

typedef pointer pointer __((offset(0x58)));

typedef pointer pointer __((offset(0x15c)));

typedef ulong size_t;

typedef sdword fx32;

typedef ushort wint_t;

typedef uchar REGType8;

typedef uchar GXScrFmt256Bmp;

typedef dword REGType32;

typedef qword vu64;


// WARNING! conflicting data type names: /DWARF/size_t - /stddef.h/size_t

typedef ushort GXRgb;

typedef uchar WBTPacketCommand;

typedef uint GXSt;

typedef int WBTBlockSeqNo;

typedef sdword s32;

typedef word vu16;

typedef dword u32;

typedef ushort WBTAidBitmap;

typedef uint GXTexPlttBaseParam;

typedef uint __file_handle;

typedef uint clock_t;

typedef dword MATHCRC32Context;

typedef qword REGType64v;

typedef uint FSOverlayID;

typedef dword REGType32v;

typedef int WBTBlockSize;

typedef uchar REGType8v;

typedef qword REGType64;

typedef word MATHCRC16Context;

typedef word REGType16v;

typedef ulonglong OSTick;

typedef uint PMWakeUpTrigger;

typedef int OSHeapHandle;

typedef ushort ENVType;

typedef uint SOCLInAddr;

typedef uint WBTBlockId;

typedef ushort MATHChecksum8Context;

typedef int ptrdiff_t;

typedef uchar GXScrFmtAffine;

typedef uchar MATHCRC8Context;

typedef uint VecFx10;

typedef ushort GXScrFmtText;

typedef word MATHChecksum16Context;

typedef uchar WBTCommandCounter;

typedef uint VecVtxDiff;

typedef uint fpos_t;

typedef int mbstate_t;

typedef uchar vu8;

typedef word REGType16;

typedef uint VecVtx10;

typedef uint PMLogic;

typedef word u16;

typedef uint OSIrqMask;

typedef ushort Wint_t;


// WARNING! conflicting data type names: /DWARF/wchar_t - /wchar_t

typedef uchar u8;

typedef ushort GXRgba;

typedef dword vu32;

typedef sdword vs32;

typedef int time_t;

typedef uint OSIntrMode;

typedef qword u64;

typedef int BOOL;

typedef uint CPSInAddr;

typedef ushort wctype_t;

typedef struct OSThreadInfo OSThreadInfo, *POSThreadInfo;

typedef struct _OSThread _OSThread, *P_OSThread;

typedef struct OSContext OSContext, *POSContext;

typedef enum @enum {
    DWCHTTP_E_NOERR=0,
    DWCHTTP_POST=0,
    DWC_PROF_REGION_SELF=0,
    DWC_PROF_STATE_NOT_INITIALIZED=0,
    OS_THREAD_STATE_WAITING=0,
    DWCHTTP_E_MEMERR=1,
    DWCHTTP_GET=1,
    DWC_PROF_STATE_OPERATING=1,
    OS_THREAD_STATE_READY=1,
    DWCHTTP_E_DNSERR=2,
    DWC_PROF_STATE_SUCCESS=2,
    OS_THREAD_STATE_TERMINATED=2,
    DWCHTTP_E_CONERR=3,
    DWC_PROF_STATE_FAIL=3,
    DWCHTTP_E_SENDTOUT=4,
    DWCHTTP_E_SENDERR=5,
    DWCHTTP_E_RECVTOUT=6,
    DWCHTTP_E_ABORT=7,
    DWCHTTP_E_FINISH=8,
    DWCHTTP_E_MAX=9,
    DWC_PROF_REGION_ALL=255
} @enum;

typedef struct _OSThreadQueue _OSThreadQueue, *P_OSThreadQueue;

typedef struct _OSThreadLink _OSThreadLink, *P_OSThreadLink;

typedef struct OSMutex OSMutex, *POSMutex;

typedef struct _OSMutexQueue _OSMutexQueue, *P_OSMutexQueue;

typedef struct OSiAlarm OSiAlarm, *POSiAlarm;

typedef struct CPContext CPContext, *PCPContext;

typedef struct _OSMutexLink _OSMutexLink, *P_OSMutexLink;

struct _OSThreadQueue {
    struct _OSThread *head;
    struct _OSThread *tail;
};

struct CPContext {
    unsigned long long div_numer;
    unsigned long long div_denom;
    unsigned long long sqrt;
    unsigned short div_mode;
    unsigned short sqrt_mode;
};

struct OSContext {
    unsigned long cpsr;
    unsigned long r[13];
    unsigned long sp;
    unsigned long lr;
    unsigned long pc_plus4;
    unsigned long sp_svc;
    struct CPContext cp_context;
};

struct _OSMutexQueue {
    struct OSMutex *head;
    struct OSMutex *tail;
};

struct _OSThreadLink {
    struct _OSThread *prev;
    struct _OSThread *next;
};

struct _OSThread {
    struct OSContext context;
    enum @enum state;
    struct _OSThread *next;
    unsigned long id;
    unsigned long priority;
    void *profiler;
    struct _OSThreadQueue *queue;
    struct _OSThreadLink link;
    struct OSMutex *mutex;
    struct _OSMutexQueue mutexQueue;
    unsigned long stackTop;
    unsigned long stackBottom;
    unsigned long stackWarningOffset;
    struct _OSThreadQueue joinQueue;
    void *specific[3];
    struct OSiAlarm *alarmForSleep;
    void (*destructor)(void *);
    void *userParameter;
    int systemErrno;
};

struct _OSMutexLink {
    struct OSMutex *next;
    struct OSMutex *prev;
};

struct OSMutex {
    struct _OSThreadQueue queue;
    struct _OSThread *thread;
    long count;
    struct _OSMutexLink link;
};

struct OSiAlarm {
    void (*handler)(void *);
    void *arg;
    unsigned long tag;
    unsigned long long fire;
    struct OSiAlarm *prev;
    struct OSiAlarm *next;
    unsigned long long period;
    unsigned long long start;
};

struct OSThreadInfo {
    unsigned short isNeedRescheduling;
    unsigned short irqDepth;
    struct _OSThread *current;
    struct _OSThread *list;
    void *switchCallback;
};

typedef struct CPSMd5Ctx CPSMd5Ctx, *PCPSMd5Ctx;

struct CPSMd5Ctx {
    unsigned long state[4];
    unsigned long count[2];
    uchar buffer[64];
};

typedef struct _CPSSslConnection _CPSSslConnection, *P_CPSSslConnection;

typedef struct _CPSSslConnection SOCSslConnection;

typedef union anon_union_72_2_d93c2505_for_common1 anon_union_72_2_d93c2505_for_common1, *Panon_union_72_2_d93c2505_for_common1;

typedef union CPSCipherCtx CPSCipherCtx, *PCPSCipherCtx;

typedef struct CPSSha1Ctx CPSSha1Ctx, *PCPSSha1Ctx;

typedef struct anon_struct_20_5_c7316894 anon_struct_20_5_c7316894, *Panon_struct_20_5_c7316894;

typedef struct anon_struct_20_5_c7316894 SOCLCaInfo;

typedef struct CPSPrivateKey CPSPrivateKey, *PCPSPrivateKey;

typedef struct CPSCertificate CPSCertificate, *PCPSCertificate;

typedef struct CPSRc4Ctx CPSRc4Ctx, *PCPSRc4Ctx;

struct anon_struct_20_5_c7316894 {
    char *dn;
    int modulus_len;
    uchar *modulus;
    int exponent_len;
    uchar *exponent;
};

struct CPSPrivateKey {
    int modulus_len;
    uchar *modulus;
    int prime1_len;
    uchar *prime1;
    int prime2_len;
    uchar *prime2;
    int exponent1_len;
    uchar *exponent1;
    int exponent2_len;
    uchar *exponent2;
    int coefficient_len;
    uchar *coefficient;
};

struct CPSSha1Ctx {
    unsigned long state[5];
    unsigned long count[2];
    uchar buffer[64];
};

struct CPSRc4Ctx {
    uchar x;
    uchar y;
    uchar m[256];
    unsigned short padding;
};

union CPSCipherCtx {
    struct CPSRc4Ctx rc4_ctx;
};

union anon_union_72_2_d93c2505_for_common1 {
    uchar sessionID[32];
    uchar key_block[72];
};

struct _CPSSslConnection {
    uchar master_secret[48];
    uchar session_cached;
    uchar reuse_session;
    unsigned short method;
    uchar client_random[32];
    uchar server_random[32];
    union anon_union_72_2_d93c2505_for_common1 common1;
    uchar *send_mac;
    uchar *send_key;
    uchar *send_iv;
    union CPSCipherCtx send_cipher;
    uchar send_seq[8];
    uchar *rcv_mac;
    uchar *rcv_key;
    uchar *rcv_iv;
    union CPSCipherCtx rcv_cipher;
    uchar rcv_seq[8];
    struct CPSSha1Ctx sha1_hash;
    struct CPSSha1Ctx sha1_hash_tmp;
    struct CPSMd5Ctx md5_hash;
    struct CPSMd5Ctx md5_hash_tmp;
    uchar server;
    uchar state;
    uchar inbuf_decrypted;
    uchar padding2;
    int sig_algorithm;
    int pub_algorithm;
    uchar *hash_start;
    uchar *hash_end;
    uchar hash_val[20];
    int hash_len;
    SOCLCaInfo midca_info;
    uchar modulus[256];
    unsigned long modulus_len;
    uchar exponent[8];
    int exponent_len;
    uchar *signature;
    int signature_len;
    uchar seen_validity;
    uchar seen_pub_algorithm;
    uchar seen_attr;
    uchar date_ok;
    char issuer[256];
    char subject[256];
    char cn[80];
    char *server_name;
    uchar *cert;
    int certlen;
    unsigned long cur_date;
    int (*auth_callback)(int, struct _CPSSslConnection *, int);
    SOCLCaInfo **ca_info;
    int ca_builtins;
    struct CPSPrivateKey *my_key;
    struct CPSCertificate *my_certificate;
    uchar *inbuf;
    long inbuf_len;
    long inbuf_pnt;
};

struct CPSCertificate {
    int certificate_len;
    uchar *certificate;
};

typedef struct _OSThreadLink OSThreadLink;

typedef struct DWCHttpLabelValue DWCHttpLabelValue, *PDWCHttpLabelValue;

struct DWCHttpLabelValue {
    char *label;
    char *value;
};

typedef struct _CPSSoc _CPSSoc, *P_CPSSoc;

typedef struct CPSSocBuf CPSSocBuf, *PCPSSocBuf;

struct CPSSocBuf {
    unsigned long size;
    uchar *data;
};

struct _CPSSoc {
    struct _OSThread *thread;
    unsigned long block_type;
    uchar state;
    uchar ssl;
    unsigned short local_port;
    void *con;
    unsigned long when;
    unsigned long local_ip_real;
    unsigned short remote_port;
    unsigned short remote_port_bound;
    unsigned long remote_ip;
    unsigned long remote_ip_bound;
    unsigned long ackno;
    unsigned long seqno;
    unsigned short remote_win;
    unsigned short remote_mss;
    unsigned long remote_ackno;
    unsigned long ackrcvd;
    int (*udpread_callback)(uchar *, unsigned long, struct _CPSSoc *);
    struct CPSSocBuf rcvbuf;
    unsigned long rcvbufp;
    struct CPSSocBuf sndbuf;
    struct CPSSocBuf linbuf;
    struct CPSSocBuf outbuf;
    unsigned long outbufp;
};

typedef struct DWCProfWorkarea DWCProfWorkarea, *PDWCProfWorkarea;

typedef struct DWCHttp DWCHttp, *PDWCHttp;

typedef struct DWCHttpParam DWCHttpParam, *PDWCHttpParam;

typedef struct DWCHttpBuffer DWCHttpBuffer, *PDWCHttpBuffer;

struct DWCHttpBuffer {
    char *buffer;
    char *write_index;
    char *buffer_tail;
    unsigned long length;
};

struct DWCHttpParam {
    char *url;
    enum @enum action;
    unsigned long len_recvbuf;
    void * (*alloc)(unsigned long, long);
    void (*free)(unsigned long, void *, long);
    int ignoreca;
    int timeout;
};

struct DWCHttp {
    uchar stack[4096];
    uchar initflag;
    struct DWCHttpParam param;
    enum @enum error;
    char url[256];
    char *hostname;
    char *filepath;
    unsigned long hostip;
    int ssl_enabled;
    unsigned short port;
    struct _CPSSoc soc;
    struct _CPSSslConnection con;
    uchar *lowrecvbuf;
    uchar *lowsendbuf;
    unsigned long lowentropydata[8];
    int num_postitem;
    struct DWCHttpBuffer req;
    struct DWCHttpBuffer rep;
    struct OSMutex content_len_mutex;
    int content_len;
    int receivedbody_len;
    struct DWCHttpLabelValue labelvalue[32];
    struct _OSThread thread;
    struct OSMutex mutex;
    int abort;
};

struct DWCProfWorkarea {
    struct DWCHttp http;
    int http_initialized;
    char url[257];
    char *result;
    int *badwordsnum;
    int wordsnum;
};

typedef struct _CPSSslConnection CPSSslConnection;

typedef struct OSiAlarm OSAlarm;

typedef struct _CPSSoc CPSSoc;

typedef struct anon_struct_20_5_c7316894 SOCCaInfo;

typedef struct _OSMutexLink OSMutexLink;

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

typedef struct _OSThread OSThread;

typedef struct _OSThreadQueue OSThreadQueue;

typedef struct anon_struct_20_5_c7316894 CPSCaInfo;

typedef struct _CPSSslConnection SOCLSslConnection;

typedef struct _OSMutexQueue OSMutexQueue;

typedef struct Elf32_Rela Elf32_Rela, *PElf32_Rela;

struct Elf32_Rela {
    dword r_offset; // location to apply the relocation action
    dword r_info; // the symbol table index and the type of relocation
    dword r_addend; // a constant addend used to compute the relocatable field value
};

typedef enum Elf_SectionHeaderType_ARM {
    SHT_NULL=0,
    SHT_PROGBITS=1,
    SHT_SYMTAB=2,
    SHT_STRTAB=3,
    SHT_RELA=4,
    SHT_HASH=5,
    SHT_DYNAMIC=6,
    SHT_NOTE=7,
    SHT_NOBITS=8,
    SHT_REL=9,
    SHT_SHLIB=10,
    SHT_DYNSYM=11,
    SHT_INIT_ARRAY=14,
    SHT_FINI_ARRAY=15,
    SHT_PREINIT_ARRAY=16,
    SHT_GROUP=17,
    SHT_SYMTAB_SHNDX=18,
    SHT_ANDROID_REL=1610612737,
    SHT_ANDROID_RELA=1610612738,
    SHT_GNU_ATTRIBUTES=1879048181,
    SHT_GNU_HASH=1879048182,
    SHT_GNU_LIBLIST=1879048183,
    SHT_CHECKSUM=1879048184,
    SHT_SUNW_move=1879048186,
    SHT_SUNW_COMDAT=1879048187,
    SHT_SUNW_syminfo=1879048188,
    SHT_GNU_verdef=1879048189,
    SHT_GNU_verneed=1879048190,
    SHT_GNU_versym=1879048191,
    SHT_ARM_EXIDX=1879048193,
    SHT_ARM_PREEMPTMAP=1879048194,
    SHT_ARM_ATTRIBUTES=1879048195,
    SHT_ARM_DEBUGOVERLAY=1879048196,
    SHT_ARM_OVERLAYSECTION=1879048197
} Elf_SectionHeaderType_ARM;

typedef struct Elf32_Sym Elf32_Sym, *PElf32_Sym;

struct Elf32_Sym {
    dword st_name;
    dword st_value;
    dword st_size;
    byte st_info;
    byte st_other;
    word st_shndx;
};

typedef struct Elf32_Shdr Elf32_Shdr, *PElf32_Shdr;

struct Elf32_Shdr {
    dword sh_name;
    enum Elf_SectionHeaderType_ARM sh_type;
    dword sh_flags;
    dword sh_addr;
    dword sh_offset;
    dword sh_size;
    dword sh_link;
    dword sh_info;
    dword sh_addralign;
    dword sh_entsize;
};

typedef struct Elf32_Ehdr Elf32_Ehdr, *PElf32_Ehdr;

struct Elf32_Ehdr {
    byte e_ident_magic_num;
    char e_ident_magic_str[3];
    byte e_ident_class;
    byte e_ident_data;
    byte e_ident_version;
    byte e_ident_osabi;
    byte e_ident_abiversion;
    byte e_ident_pad[7];
    word e_type;
    word e_machine;
    dword e_version;
    dword e_entry;
    dword e_phoff;
    dword e_shoff;
    dword e_flags;
    word e_ehsize;
    word e_phentsize;
    word e_phnum;
    word e_shentsize;
    word e_shnum;
    word e_shstrndx;
};




// WARNING: Enum "@enum": Some values do not have unique names

undefined4
DWCi_CheckProfanityAsync
          (int param_1,int param_2,int param_3,int param_4,char *param_5,int *param_6,
          undefined4 param_7)

{
  char *pcVar1;
  int iVar2;
  size_t sVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  char *local_e0;
  undefined4 local_dc;
  undefined4 local_d8;
  code *local_d4;
  code *local_d0;
  undefined4 local_cc;
  int local_c8;
  undefined auStack_c4 [148];
  
  iVar5 = 0;
  if ((((param_1 == 0) || (param_2 < 1)) || (0x32 < param_2)) ||
     ((param_5 == (char *)0x0 || (param_6 == (int *)0x0)))) {
    OSi_Panic(_13323,0x4b,_13324);
  }
  else {
    iVar6 = 0;
    for (iVar4 = 0; iVar4 < param_2; iVar4 = iVar4 + 1) {
      sVar3 = wcslen(*(wchar_t **)(param_1 + iVar4 * 4));
      iVar6 = iVar6 + sVar3 + 1;
    }
    iVar4 = iVar6 + -1;
    if ((iVar4 < 1) || (500 < iVar4)) {
      OSi_Panic(_13323,0x58,_13324);
    }
    else if (intwork == (DWCProfWorkarea *)0x0) {
      MI_CpuClear8(param_5,param_2);
      intwork = (DWCProfWorkarea *)DWC_Alloc(0,0x1d28);
      if (intwork != (DWCProfWorkarea *)0x0) {
        MI_CpuClear8(intwork,0x1d28);
        intwork->http_initialized = 0;
        MI_CpuClear8(auStack_c4,0x94);
        intwork->result = param_5;
        intwork->badwordsnum = param_6;
        intwork->wordsnum = param_2;
        strncpy(intwork->url,DWCauthhttpparam,0x100);
        intwork->url[0x100] = '\0';
        pcVar1 = strstr(intwork->url,(char *)&_13325);
        if (pcVar1 != (char *)0x0) {
          MI_CpuCopy8(&_13326,pcVar1,4);
          local_e0 = intwork->url;
          local_dc = 0;
          local_d8 = 0x400;
          local_d4 = DWCi_Prof_Alloc;
          local_d0 = DWCi_Prof_Free;
          local_cc = 0;
          local_c8 = param_4;
          if (param_4 < 1) {
            local_c8 = 10000;
          }
          iVar2 = DWC_Http_Create(intwork,&local_e0);
          if (iVar2 == 0) {
            intwork->http_initialized = 1;
            iVar2 = DWC_Auth_GetCalInfoFromNVRAM(auStack_c4);
            if (((((iVar2 != 0) &&
                  (iVar2 = DWC_Auth_SetCalInfoToHttp(intwork,auStack_c4,DWCishotspot), iVar2 != 0))
                 && (iVar2 = DWC_Http_Add_PostBase64Item(intwork,_13327,param_7,1), iVar2 == 0)) &&
                (iVar2 = DWC_Http_Add_PostBase64Item(intwork,&_13328,_13329,8), iVar2 == 0)) &&
               (((param_3 == 0 ||
                 (iVar2 = DWC_Http_Add_PostBase64Item(intwork,_13330,param_3,4), iVar2 == 0)) &&
                ((iVar2 = DWC_Http_Add_Body(intwork,_13331), iVar2 == 0 &&
                 (iVar5 = DWC_Alloc(0,iVar6 * 2), iVar5 != 0)))))) {
              iVar2 = 0;
              for (iVar6 = 0; iVar6 < param_2; iVar6 = iVar6 + 1) {
                wcscpy((wchar_t *)(iVar5 + iVar2 * 2),*(wchar_t **)(param_1 + iVar6 * 4));
                sVar3 = wcslen(*(wchar_t **)(param_1 + iVar6 * 4));
                iVar2 = iVar2 + sVar3;
                if (iVar4 <= iVar2) break;
                *(undefined2 *)(iVar5 + iVar2 * 2) = 9;
                iVar2 = iVar2 + 1;
              }
              iVar4 = DWC_Http_Add_PostBase64Item(intwork,_13332,iVar5,iVar4 * 2);
              if (iVar4 == 0) {
                DWC_Free(0,iVar5,0);
                iVar5 = 0;
                iVar4 = DWC_Http_FinishHeader(intwork);
                if (iVar4 == 0) {
                  OS_GetCurrentThread();
                  iVar4 = OS_GetThreadPriority();
                  DWC_Http_StartThread(intwork,iVar4 + -1);
                  if ((intwork->http).thread.id != 0) {
                    return 1;
                  }
                }
              }
            }
          }
        }
      }
      if (iVar5 != 0) {
        DWC_Free(0,iVar5,0);
      }
      if (intwork != (DWCProfWorkarea *)0x0) {
        if (intwork->http_initialized == 1) {
          DWC_Http_Destroy(intwork);
        }
        DWC_Free(0,intwork,0);
        intwork = (DWCProfWorkarea *)0x0;
      }
    }
  }
  return 0;
}



void MI_CpuClear8(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  MI_CpuFill8(param_1,0,param_2,param_4,param_4);
  return;
}



undefined4 OS_GetCurrentThread(void)

{
  int iVar1;
  
  iVar1 = OS_GetThreadInfo();
  return *(undefined4 *)(iVar1 + 4);
}



undefined * OS_GetThreadInfo(void)

{
  return &OSi_ThreadInfo;
}



void DWC_CheckProfanityAsync
               (undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6)

{
  undefined local_1c [4];
  undefined4 local_18;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  local_1c[0] = DAT_027ffe0f;
  local_18 = param_4;
  local_10 = param_1;
  local_c = param_2;
  local_8 = param_3;
  local_4 = param_4;
  DWCi_CheckProfanityAsync(param_1,param_2,param_3,param_4,param_5,param_6,local_1c);
  return;
}



void DWC_CheckProfanityExAsync
               (undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined4 param_5,undefined4 param_6,int param_7)

{
  undefined local_1c [4];
  undefined4 local_18;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  if (param_7 == 0xff) {
    local_1c[0] = 0x41;
  }
  else {
    local_1c[0] = DAT_027ffe0f;
  }
  local_18 = param_4;
  local_10 = param_1;
  local_c = param_2;
  local_8 = param_3;
  local_4 = param_4;
  DWCi_CheckProfanityAsync(param_1,param_2,param_3,param_4,param_5,param_6,local_1c);
  return;
}



// WARNING: Enum "@enum": Some values do not have unique names

undefined4 DWC_CheckProfanityProcess(void)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  
  if ((intwork == (DWCProfWorkarea *)0x0) || ((intwork->http).thread.id == 0)) {
    return 0;
  }
  iVar1 = OS_IsThreadTerminated(&(intwork->http).thread);
  if (iVar1 != 1) {
    return 1;
  }
  uVar2 = DWC_AC_GetApType();
  if ((uVar2 == 0xff) || (99 < uVar2)) {
    uVar2 = 99;
  }
  switch((intwork->http).error) {
  case DWCHTTP_E_NOERR:
    break;
  case DWCHTTP_E_MEMERR:
    break;
  case DWCHTTP_E_DNSERR:
    DWCi_SetError(8,-0xcb84 - uVar2);
    uVar3 = 3;
    goto LAB_0001074c;
  case DWCHTTP_E_CONERR:
    goto LAB_0001071c;
  case DWCHTTP_E_SENDTOUT:
    goto LAB_0001071c;
  case DWCHTTP_E_SENDERR:
    break;
  case DWCHTTP_E_RECVTOUT:
LAB_0001071c:
    DWCi_SetError(0x13,0xffff7f0e);
    uVar3 = 3;
    goto LAB_0001074c;
  case DWCHTTP_E_ABORT:
    break;
  case DWCHTTP_E_FINISH:
    iVar1 = DWCi_Prof_ParseResponse();
    if (iVar1 == 0) {
      uVar3 = 3;
    }
    else {
      uVar3 = 2;
    }
    goto LAB_0001074c;
  }
  DWCi_SetError(0x13,0xffff7efa);
  uVar3 = 3;
LAB_0001074c:
  DWC_Http_Destroy(intwork);
  DWC_Free(0,intwork,0);
  intwork = (DWCProfWorkarea *)0x0;
  return uVar3;
}



// WARNING: Enum "@enum": Some values do not have unique names

undefined4 DWCi_Prof_ParseResponse(void)

{
  DWCProfWorkarea *pDVar1;
  int iVar2;
  char *__nptr;
  undefined4 in_r3;
  char acStack_50 [4];
  char local_4c [52];
  undefined4 uStack_18;
  
  pDVar1 = intwork;
  uStack_18 = in_r3;
  iVar2 = DWC_Http_ParseResult(intwork,0);
  if (iVar2 != 0) {
    iVar2 = DWC_Http_GetResult(pDVar1,&_13377);
    if (iVar2 != 0) {
      DWC_Auth_SetNasTimeDiff(iVar2);
    }
    __nptr = (char *)DWC_Http_GetResult(pDVar1,_13378);
    if ((((__nptr != (char *)0x0) && (iVar2 = atoi(__nptr), errno != 0x22)) && (iVar2 == 200)) &&
       ((iVar2 = DWC_Http_GetBase64DecodedResult(pDVar1,_13379,acStack_50,4), 0 < iVar2 &&
        (iVar2 = atoi(acStack_50), errno != 0x22)))) {
      if ((iVar2 != 1) && ((iVar2 < 100 || (iVar2 == 100)))) {
        DWC_Http_GetBase64DecodedResult(pDVar1,_13380,local_4c,0x33);
        *intwork->badwordsnum = 0;
        for (iVar2 = 0; iVar2 < intwork->wordsnum; iVar2 = iVar2 + 1) {
          if (local_4c[iVar2] == '0') {
            intwork->result[iVar2] = '\0';
          }
          else {
            intwork->result[iVar2] = '\x01';
            *intwork->badwordsnum = *intwork->badwordsnum + 1;
          }
        }
        return 1;
      }
      DWCi_SetError(0x12,-33000 - iVar2);
      return 0;
    }
  }
  DWCi_SetError(0x12,0xffff7f04);
  return 0;
}



void DWCi_Prof_Alloc(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  DWC_Alloc(0,param_2,param_3,param_4,param_4);
  return;
}



void DWCi_Prof_Free(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  DWC_Free(0,param_2,param_3,param_4,param_4);
  return;
}


