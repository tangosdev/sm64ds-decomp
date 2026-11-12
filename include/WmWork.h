#ifndef WMWORK_H
#define WMWORK_H

#include "nitro/wm/wm.h"

/* NitroSDK WM views over the wireless work buffer data_020a9db4 (0x3700
 * bytes, carved by MB_Init), the multiboot library's sWmWork in
 * src/engine/mb/MB_WmBase.cpp. SDK field names where the SDK has one; the
 * evidence for each is above WmWork. WMBssDesc, WMParentParam and WMStatus
 * come from nitro/wm/wm.h. Plain C types: some consumers do not include types.h. */
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
    WMBssDesc bssDesc;        /* 0x000 */
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
 *   parent/childMaxSize   set by MB_SetParentCommParam, copied into parentParam
 *   sendBuf/recvBuf, sendBufSize/recvBufSize   WM_StartMPEx
 *   stateCallback   WM_Initialize / WM_SetIndCallback / WM_Reset callback
 *   mpSending       set while WM_SetMPDataToPort is pending, cleared in its
 *                   callback (id 0xf)
 *   initialized     set after WM_Initialize, cleared in the WM_End callback
 *   userCallback    every DISPATCH; MB_StartParent installs MBi_CommParentCallback
 *   connectBssDesc  WM_StartConnect's pInfo (id 0xc)
 *   mode            MB_StartParent sets 1 (parent); MBi_SendMP's case 2
 *                   sends through this (child) callback
 *   resetting       set by MBi_CommEnd before WM_Reset; while set the
 *                   callbacks only accept the RESET and END results
 *   mpStarted       set on WM_StartMP's MP_START state (0xa)
 *   childBitmap     status.child_bitmap after a parent-side (dis)connect, 1
 *                   on the child's CONNECTED; gates MBi_IsSendEnabled
 *   mpContinuous    nonzero passes mpFreq 0 (continuous) to WM_StartMP
 *   parentCount, lastParentIndex   the parents[] fill count / last hit
 *   aid             WMStartConnectCallback.aid on CONNECTED
 *   scanContinue    cleared by MBi_CommEnd; scanning stops when 0
 *   scanNextChannel when set, changeScanChannel steps WMScanParam.channel
 *   uname, gname    MB_Init (MB_Init) copies the MBUserInfo name and
 *                   its static L"multiboot" (data_0209a084, stepped as it is
 *                   read) into them; its loops run up to 10 and 16 halfwords,
 *                   so the user name can overrun into gname
 *   status.mp_flag  MBi_ParentCallback starts MP on CONNECTED while it is 0
 * status is WM_ReadStatus's buffer, 0xa00 bytes for the 0x930-byte WMStatus. */
typedef struct {
    unsigned char wmSysBuf[0x1300];    /* 0x0000 */
    WMParentParam parentParam;         /* 0x1300 */
    WMBssDesc scanBuf;                 /* 0x1340 */
    unsigned short parentMaxSize;      /* 0x1400 */
    unsigned short childMaxSize;       /* 0x1402 */
    char *sendBuf;                     /* 0x1404 */
    char *recvBuf;                     /* 0x1408 */
    void (*stateCallback)(void *);     /* 0x140c */
    unsigned char mpSending;           /* 0x1410 */
    unsigned char initialized;         /* 0x1411 */
    unsigned char pad_1412[6];         /* 0x1412 */
    WMStatus status;                   /* 0x1418 */
    unsigned char pad_1d48[0xd0];      /* 0x1d48 */
    unsigned short sendBufSize;        /* 0x1e18 */
    unsigned short recvBufSize;        /* 0x1e1a */
    void (*userCallback)(int, void *); /* 0x1e1c */
    struct WMBssDesc *connectBssDesc;  /* 0x1e20 */
    unsigned short mode;               /* 0x1e24 */
    unsigned short resetting;          /* 0x1e26 */
    unsigned short mpStarted;          /* 0x1e28 */
    unsigned short childBitmap;        /* 0x1e2a */
    unsigned short mpContinuous;       /* 0x1e2c */
    unsigned char pad_1e2e[2];         /* 0x1e2e */
    unsigned char uname[8];            /* 0x1e30 */
    unsigned char gname[16];           /* 0x1e38 */
    unsigned char pad_1e48[0x98];      /* 0x1e48 */
    unsigned short parentCount;        /* 0x1ee0 */
    unsigned short aid;                /* 0x1ee2 */
    int scanContinue;                  /* 0x1ee4 */
    int scanNextChannel;               /* 0x1ee8 */
    int lastParentIndex;               /* 0x1eec */
    unsigned char pad_1ef0[0x10];      /* 0x1ef0 */
    WmParentRecord parents[16];        /* 0x1f00 */
} WmWork;                              /* 0x3700 */

typedef char WmStartScanCallback_size_must_be_0xb8[sizeof(WmStartScanCallback) == 0xb8 ? 1 : -1];
typedef char WmParentRecord_size_must_be_0x180[sizeof(WmParentRecord) == 0x180 ? 1 : -1];
#include "ds_width.h"
#ifndef SM64DS_HOST_64
typedef char WmWork_size_must_be_0x3700[sizeof(WmWork) == 0x3700 ? 1 : -1];
#endif

/* The work buffer through its view; each consumer declares data_020a9db4. */
#define WM_WORK ((WmWork *)data_020a9db4)

#endif
