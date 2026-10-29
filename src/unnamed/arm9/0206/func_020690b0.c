#include "types.h"
#include "WmWork.h"
/* func_020690b0 @ 0x020690b0 (arm9, size 0x868)   [mwccarm 1.2/sp2p3, 2004/b56]
 * Wireless (WM) child-side callback: scan, connect, then start MP. Each
 * PARENT_FOUND result lands in WmWork.parents[]: the record's first 0xc0 bytes
 * are the WMBssDesc DMA'd from the scan buffer, and +0xc0 is a copy of the
 * 0xb8-byte WMStartScanCallback itself (macAddress at +0xa, gameInfoLength at
 * +0x36, gameInfo at +0x38). An earlier banner called that copy a WMBssDesc;
 * the MultiCopyHalf source (the callback) and length (0xb8) say otherwise.
 *
 * WmWork field evidence (WM API ids from each wrapper's WM_SetCallbackTable):
 *   wmSysBuf        buffer arg of func_02062240 (id 0, WM_Initialize)
 *   parentParam     func_020620b0 (id 7, WM_SetParentParameter); KS_Flag at
 *                   +0x12 and the max sizes at +0x32/+0x34 are its size check;
 *                   func_02068844 packs entry/MB/KS/CS into WM_ATTR_FLAG_*
 *   scanBuf         WMScanParam.scanBuf; DMA'd into parents[i].bssDesc
 *   parent/childMaxSize   set by func_02068dc8, copied into parentParam
 *   sendBuf/recvBuf, sendBufSize/recvBufSize   WM_StartMP (func_020625fc)
 *   stateCallback   WM_Initialize / WM_SetIndCallback / WM_Reset callback
 *   mpSending       set while WM_SetMPDataToPort is pending, cleared in its
 *                   callback (id 0xf)
 *   initialized     set after WM_Initialize, cleared in the WM_End callback
 *   userCallback    every DISPATCH; func_02068ae8 installs func_02066a94
 *   connectBssDesc  WM_StartConnect's pInfo (func_02061e38, id 0xc)
 *   mode            func_02068ae8 sets 1 (parent); func_020688fc's case 2
 *                   sends through this (child) callback
 *   resetting       set by func_02068a74 before WM_Reset; while set the
 *                   callbacks only accept the RESET and END results
 *   mpStarted       set on WM_StartMP's MP_START state (0xa)
 *   childBitmap     WmStatus.child_bitmap after a parent-side (dis)connect, 1
 *                   on the child's CONNECTED; gates func_0206a230
 *   mpContinuous    nonzero passes mpFreq 0 (continuous) to WM_StartMP
 *   parentCount, lastParentIndex   the parents[] fill count / last hit
 *   aid             WMStartConnectCallback.aid on CONNECTED
 *   scanContinue    cleared by func_02068a74; scanning stops when 0
 *   scanNextChannel when set, func_0206a29c steps WMScanParam.channel
 * child_bitmap's offset inside WMStatus (+0x17e) is from this code, not the SDK.
 */
extern char *data_020a9db4;
extern char *data_020a9db8;
extern u16 data_020a9db0;
extern char data_020a9dc0[];

extern int func_020621b8(void *fn);
extern void func_0206879c(int a, int b);
extern int func_02061c20(void *p);
extern int func_02063574(void *fn, int a, int b, int c);
extern int func_02061ee4(void *fn, void *p);
extern int func_02061e38(void *fn, int a, int b);
extern int func_02061c88(int a, void *fn, int c);
extern int func_020625fc(void *fn, int a, u16 b, int c, u16 d, u16 e, int f,
                         int g, int h, int i, int j);
extern int func_0206a230(void);
extern int func_0206a29c(void *p);
extern int func_02068a74(void);
extern void func_02069918(void);
extern void _ZN4CP1519InvalidateDataCacheEjj(u32 addr, u32 n);
extern void DMASyncHalfTransfer(u32 ch, void *src, void *dst, u32 n);
extern void MultiCopyHalf(void *src, void *dst, u32 n);

void func_020690b0(char *arg0);

/* WMScanParam (data_020a9dc0), WM_StartScan's argument; func_0206a29c steps
 * its channel through the allowed-channel mask. */
typedef struct {
    WmBssDesc *scanBuf; /* 0x0 */
    u16 channel;        /* 0x4 */
    u16 maxChannelTime; /* 0x6 */
    u16 bssid[3];       /* 0x8: SDK u8[6]; all 0xffff = any parent */
} WmScanParam;

#define SCAN_PARAM ((WmScanParam *)data_020a9dc0)

/* Leading fields of WMStartConnectCallback. */
typedef struct {
    u16 apiid;    /* 0x0 */
    u16 errcode;  /* 0x2 */
    u16 wlCmdID;  /* 0x4 */
    u16 wlResult; /* 0x6 */
    u16 state;    /* 0x8 */
    u16 aid;      /* 0xa */
} WmStartConnectCallback;

/* The 0x80-byte WMGameInfo, copied as one struct. */
typedef struct {
    u16 w[0x40];
} GameInfo;

static inline void RecordScannedParent(WmWork *const w, char *arg0) {
    WmStartScanCallback *const cb = (WmStartScanCallback *)arg0;
    WmParentRecord *tbl = w->parents;
    int i;
    for (i = 0; i < w->parentCount; i++) {
        if (cb->macAddress[0] == tbl[i].scan.macAddress[0] &&
            cb->macAddress[1] == tbl[i].scan.macAddress[1] &&
            cb->macAddress[2] == tbl[i].scan.macAddress[2]) {
            tbl[i].scan.gameInfoLength = cb->gameInfoLength;
            *(GameInfo *)tbl[i].scan.gameInfo = *(GameInfo *)cb->gameInfo;
            _ZN4CP1519InvalidateDataCacheEjj((u32)&w->parents[i], 0xc0);
            DMASyncHalfTransfer(data_020a9db0, &w->scanBuf, &w->parents[i], 0xc0);
            w->lastParentIndex = i;
            return;
        }
    }
    if (i < 0x10) {
        w->parentCount = i + 1;
        MultiCopyHalf(arg0, &tbl[i].scan, 0xb8);
        _ZN4CP1519InvalidateDataCacheEjj((u32)&w->parents[i], 0xc0);
        DMASyncHalfTransfer(data_020a9db0, &w->scanBuf, &w->parents[i], 0xc0);
        w->lastParentIndex = i;
    }
}

#define DISPATCH(a, b) w->userCallback((a), (b))

void func_020690b0(char *arg0)
{
    WmWork *const w = (WmWork *)data_020a9db4;

    if (w->resetting != 0) {
        if (*(u16 *)arg0 == 1) {
            if (*(u16 *)(arg0 + 2) == 0) {
                func_0206879c(2, func_020621b8((void *)func_020690b0));
                return;
            }
            w->resetting = 0;
            DISPATCH(0x100, arg0);
            return;
        }
        if (*(u16 *)arg0 != 2) return;
        if (*(u16 *)(arg0 + 2) == 0) {
            w->initialized = 0;
            *(u16 *)(data_020a9db8 + 0x1300 + 0x16) = 0;
            DISPATCH(0x11, arg0);
            return;
        }
        w->resetting = 0;
        DISPATCH(0x100, arg0);
        return;
    }

    switch (*(u16 *)arg0) {
    case 0:
        if (*(u16 *)(arg0 + 2) == 0) {
            DISPATCH(0x15, arg0);
            func_0206879c(0x83, func_02061c20(&w->status));
            func_0206879c(0x1d, func_02063574((void *)func_020690b0, 0xffff, 0x28, 5));
            return;
        }
        DISPATCH(0x100, arg0);
        return;

    case 0x1d:
        if (*(u16 *)(arg0 + 2) == 0) {
            SCAN_PARAM->scanBuf = &w->scanBuf;
            if (SCAN_PARAM->channel == 0)
                SCAN_PARAM->channel = 1;
            if (SCAN_PARAM->maxChannelTime == 0)
                SCAN_PARAM->maxChannelTime = 0xc8;
            SCAN_PARAM->bssid[0] = 0xffff;
            SCAN_PARAM->bssid[1] = 0xffff;
            SCAN_PARAM->bssid[2] = 0xffff;
            w->scanContinue = 1;
            w->scanNextChannel = 1;
            func_0206879c(0xa, func_02061ee4((void *)func_020690b0, data_020a9dc0));
            return;
        }
        DISPATCH(0x100, arg0);
        return;

    case 0xa:
        if (*(u16 *)(arg0 + 2) == 0) {
            switch (*(u16 *)(arg0 + 8)) {
            case 3:
                return;
            case 5:
                RecordScannedParent(w, arg0);
                DISPATCH(4, arg0);
                if (w->scanContinue == 0)
                    return;
                if (w->scanNextChannel != 0 && func_0206a29c(data_020a9dc0) == 0)
                    func_0206879c(2, func_02068a74());
                func_0206879c(0xa, func_02061ee4((void *)func_020690b0, data_020a9dc0));
                return;
            case 4:
                DISPATCH(5, arg0);
                if (w->scanContinue == 0)
                    return;
                if (w->scanNextChannel != 0 && func_0206a29c(data_020a9dc0) == 0)
                    func_0206879c(2, func_02068a74());
                func_0206879c(0xa, func_02061ee4((void *)func_020690b0, data_020a9dc0));
                return;
            default:
                DISPATCH(0x100, arg0);
                return;
            }
        } else {
            DISPATCH(0x100, arg0);
        }
        return;

    case 0xb:
        if (*(u16 *)(arg0 + 2) == 0) {
            func_0206879c(0xc, func_02061e38((void *)func_020690b0, w->connectBssDesc, 0));
            return;
        }
        DISPATCH(0x100, arg0);
        return;

    case 0xc:
        if (*(u16 *)(arg0 + 2) == 0) {
            switch (*(u16 *)(arg0 + 8)) {
            case 6:
                return;
            case 7:
                w->aid = ((WmStartConnectCallback *)arg0)->aid;
                DISPATCH(6, arg0);
                w->childBitmap = 1;
                if (func_02061c88(1, (void *)func_02069918, 0) != 0) return;
                func_0206879c(0xe, func_020625fc((void *)func_020690b0, (int)w->recvBuf,
                                                 w->recvBufSize, (int)w->sendBuf, w->sendBufSize,
                                                 (u16)(w->mpContinuous == 0), 0, 0, 0, 1, 1));
                return;
            case 9:
                DISPATCH(0xa, arg0);
                w->childBitmap = 0;
                return;
            default:
                DISPATCH(0x100, arg0);
                return;
            }
        } else {
            w->parentCount = 0;
            DISPATCH(0xb, arg0);
        }
        return;

    case 0xe:
        switch (*(u16 *)(arg0 + 4)) {
        case 0xa:
            w->mpStarted = 1;
            if (func_0206a230() == 0) return;
            DISPATCH(0x19, 0);
            return;
        case 0xc:
            return;
        case 0xd:
            return;
        default:
            DISPATCH(0x100, arg0);
            return;
        }

    case 0xf:
        w->mpSending = 0;
        if (*(u16 *)(arg0 + 2) == 0)
            DISPATCH(8, arg0);
        else if (*(u16 *)(arg0 + 2) == 9)
            DISPATCH(0x29, arg0);
        else
            DISPATCH(0x12, arg0);
        DISPATCH(0x19, 0);
        return;

    case 0x15:
        if (func_0206a230() == 0) return;
        DISPATCH(0x19, 0);
        return;

    case 0x80:
        return;

    default:
        DISPATCH(0x100, arg0);
        return;
    }
}
