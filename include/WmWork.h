#ifndef WMWORK_H
#define WMWORK_H

/* NitroSDK WM views over the wireless work buffer data_020a9db4 (0x3700
 * bytes, carved by func_02068e4c), the multiboot library's sWmWork in
 * src/engine/mb/MB_WmBase.cpp. SDK field names where the SDK has one; the
 * evidence for each is above WmWork. Plain C types: some consumers do not
 * include types.h. WmCheckParentParameter.c includes this header without
 * it, and func_020408b0, WM_GetMPReceiveBufferSize and WM_SetMPDataToPortEx,
 * which keep their own views of these structs for now, do not include it
 * either. */
typedef struct {
    unsigned char bytes[0xc0];
} WmBssDesc; /* WMBssDesc */

typedef struct {
    unsigned short apiid;          /* 0x00 */
    unsigned short errcode;        /* 0x02 */
    unsigned short wlCmdID;        /* 0x04 */
    unsigned short wlResult;       /* 0x06 */
    unsigned short state;          /* 0x08 */
    unsigned short macAddress[3];  /* 0x0a, SDK u8[6] */
    unsigned short channel;        /* 0x10 */
    unsigned short linkLevel;      /* 0x12 */
    unsigned short ssidLength;     /* 0x14 */
    unsigned short ssid[16];       /* 0x16 */
    unsigned short gameInfoLength; /* 0x36 */
    unsigned short gameInfo[0x40]; /* 0x38, WMGameInfo */
} WmStartScanCallback;             /* WMStartScanCallback, 0xb8 */

typedef struct {
    unsigned short *userGameInfo;      /* 0x00 */
    unsigned short userGameInfoLength; /* 0x04 */
    unsigned short ggid[2];            /* 0x06 */
    unsigned short tgid;               /* 0x0a */
    unsigned short entryFlag;          /* 0x0c */
    unsigned short maxEntry;           /* 0x0e */
    unsigned short multiBootFlag;      /* 0x10 */
    unsigned short KS_Flag;            /* 0x12 */
    unsigned short CS_Flag;            /* 0x14 */
    unsigned short beaconPeriod;       /* 0x16 */
    unsigned short rsv[12];            /* 0x18 */
    unsigned short channel;            /* 0x30 */
    unsigned short parentMaxSize;      /* 0x32 */
    unsigned short childMaxSize;       /* 0x34 */
    unsigned short rsv2[5];            /* 0x36 */
} WmParentParam;                       /* WMParentParam, 0x40 */
/* ggid is two u16s at 0x06 with no padding before it, so tgid..childMaxSize
 * sit 2 bytes earlier than in the public NitroSDK WMParentParam. That is
 * this ROM's SDK revision: func_02068e4c fills parentParam at these offsets
 * byte-identically before WM_SetParentParameter, and the size assert below
 * holds. Don't realign it to the public header. */

typedef struct {
    unsigned char pad_000[0x17e];
    unsigned short child_bitmap; /* 0x17e */
    unsigned char pad_180[0x880];
} WmStatus; /* WMStatus, WM_ReadStatus's buffer */

typedef struct {
    WmBssDesc bssDesc;        /* 0x000 */
    WmStartScanCallback scan; /* 0x0c0 */
    unsigned char pad_178[8];
} WmParentRecord; /* one scanned parent, 0x180 */

/* Leading fields of WMStartConnectCallback (apiid 0xc), what WM_StartConnect's
 * callback receives. */
typedef struct WmStartConnectCallback {
    unsigned short apiid;    /* 0x0 */
    unsigned short errcode;  /* 0x2: 0 is WM_ERRCODE_SUCCESS */
    unsigned short wlCmdID;  /* 0x4 */
    unsigned short wlResult; /* 0x6 */
    unsigned short state;    /* 0x8: WM_STATECODE_* */
    unsigned short aid;      /* 0xa: the AID the parent assigned */
} WmStartConnectCallback;

/* WmWork field evidence (WM API ids from each wrapper's WM_SetCallbackTable):
 *   wmSysBuf        buffer arg of WM_Initialize (id 0, WM_Initialize)
 *   parentParam     WM_SetParentParameter (id 7); KS_Flag at
 *                   +0x12 and the max sizes at +0x32/+0x34 are its size check;
 *                   MBi_GetAttribute packs entry/MB/KS/CS into WM_ATTR_FLAG_*
 *   scanBuf         WMScanParam.scanBuf; DMA'd into parents[i].bssDesc
 *   parent/childMaxSize   set by func_02068dc8, copied into parentParam
 *   sendBuf/recvBuf, sendBufSize/recvBufSize   WM_StartMPEx
 *   stateCallback   WM_Initialize / WM_SetIndCallback / WM_Reset callback
 *   mpSending       set while WM_SetMPDataToPort is pending, cleared in its
 *                   callback (id 0xf)
 *   initialized     set after WM_Initialize, cleared in the WM_End callback
 *   userCallback    every DISPATCH; func_02068ae8 installs MBi_CommParentCallback
 *   connectBssDesc  WM_StartConnect's pInfo (id 0xc)
 *   mode            func_02068ae8 sets 1 (parent); func_020688fc's case 2
 *                   sends through this (child) callback
 *   resetting       set by func_02068a74 before WM_Reset; while set the
 *                   callbacks only accept the RESET and END results
 *   mpStarted       set on WM_StartMP's MP_START state (0xa)
 *   childBitmap     WmStatus.child_bitmap after a parent-side (dis)connect, 1
 *                   on the child's CONNECTED; gates MBi_IsSendEnabled
 *   mpContinuous    nonzero passes mpFreq 0 (continuous) to WM_StartMP
 *   parentCount, lastParentIndex   the parents[] fill count / last hit
 *   aid             WMStartConnectCallback.aid on CONNECTED
 *   scanContinue    cleared by func_02068a74; scanning stops when 0
 *   scanNextChannel when set, changeScanChannel steps WMScanParam.channel
 * child_bitmap's offset inside WMStatus (+0x17e) is from this code, not the SDK. */
typedef struct {
    unsigned char wmSysBuf[0x1300];    /* 0x0000 */
    WmParentParam parentParam;         /* 0x1300 */
    WmBssDesc scanBuf;                 /* 0x1340 */
    unsigned short parentMaxSize;      /* 0x1400 */
    unsigned short childMaxSize;       /* 0x1402 */
    char *sendBuf;                     /* 0x1404 */
    char *recvBuf;                     /* 0x1408 */
    void (*stateCallback)(char *);     /* 0x140c */
    unsigned char mpSending;           /* 0x1410 */
    unsigned char initialized;         /* 0x1411 */
    unsigned char pad_1412[6];         /* 0x1412 */
    WmStatus status;                   /* 0x1418 */
    unsigned short sendBufSize;        /* 0x1e18 */
    unsigned short recvBufSize;        /* 0x1e1a */
    void (*userCallback)(int, void *); /* 0x1e1c */
    int connectBssDesc;                /* 0x1e20 */
    unsigned short mode;               /* 0x1e24 */
    unsigned short resetting;          /* 0x1e26 */
    unsigned short mpStarted;          /* 0x1e28 */
    unsigned short childBitmap;        /* 0x1e2a */
    unsigned short mpContinuous;       /* 0x1e2c */
    unsigned char pad_1e2e[0xb2];      /* 0x1e2e */
    unsigned short parentCount;        /* 0x1ee0 */
    unsigned short aid;                /* 0x1ee2 */
    int scanContinue;                  /* 0x1ee4 */
    int scanNextChannel;               /* 0x1ee8 */
    int lastParentIndex;               /* 0x1eec */
    unsigned char pad_1ef0[0x10];      /* 0x1ef0 */
    WmParentRecord parents[16];        /* 0x1f00 */
} WmWork;                              /* 0x3700 */

typedef char WmParentParam_size_must_be_0x40[sizeof(WmParentParam) == 0x40 ? 1 : -1];
typedef char WmStartScanCallback_size_must_be_0xb8[sizeof(WmStartScanCallback) == 0xb8 ? 1 : -1];
typedef char WmParentRecord_size_must_be_0x180[sizeof(WmParentRecord) == 0x180 ? 1 : -1];
typedef char WmWork_size_must_be_0x3700[sizeof(WmWork) == 0x3700 ? 1 : -1];

/* The work buffer through its view; each consumer declares data_020a9db4. */
#define WM_WORK ((WmWork *)data_020a9db4)

#endif
