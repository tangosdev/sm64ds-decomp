//cpp
/* The multiboot library's use of the wireless manager (the Nitro SDK's
 * mb_wm_base.c), arm9 .text 0x0206879c..0x0206a318.
 *
 * The parent side sets up the WmWork (data_020a9db4, the BW2 reconstruction's sWmWork;
 * ntrtwl's p_mbi_param) in
 * func_02068e4c, starts the manager in func_02068ae8 and func_02068d08, and
 * answers its replies in func_02069994. The child side scans and connects in
 * func_020690b0. func_020688fc sends one MP frame for either side, and
 * MBi_CheckWmErrcode turns a failed WM call into event 0xff for
 * WmWork.userCallback.
 *
 * File and order: the BW2 decomp's reconstruction of this file (from ntrtwl's
 * NitroSDK) defines, in source order, changeScanChannel, MBi_IsSendEnabled,
 * MBi_OnInitializeDone, MBi_EndCommon, MBi_ParentCallback,
 * MBi_ChildPortCallback, MBi_ChildCallback, MBi_GetBeaconPeriodDispersion,
 * MB_Init, MBi_IsCommSizeValid, MB_SetParentCommParam, MBi_StartCommon,
 * MBi_StartParentCore, MB_StartParent, MBi_CallReset, MBi_OnReset,
 * MBi_CommEnd, MB_End, MB_DisconnectChild, MBi_SetMaxScanTime, MBi_SetMPData,
 * MBi_SendMP, MBi_GetGgid, MBi_GetTgid, MBi_GetAttribute, MBi_IsStarted and
 * MBi_CheckWmErrcode. mwccarm emits the bodies in reverse, and the ROM run,
 * read from the top down, is that order with the newer SDK's extras missing:
 * a reconstruction counterpart for every function here, the same body shape
 * for each, and MBi_SetMaxScanTime and func_0206881c the only two out of
 * place (they sit just above MBi_IsStarted). sWmWork (data_020a9db4), the
 * scan parameter (data_020a9dc0, sScanParam) and the DMA channel
 * (data_020a9db0, sDmaNo) are that file's statics; nothing outside this run
 * writes them except through these functions.
 *
 * Names: seven functions take the SDK name where the body is the
 * reconstruction's (or the HeartGold fork's mb_comm_params.c) statement for
 * statement: MBi_IsSendEnabled, MBi_GetBeaconPeriodDispersion,
 * MBi_SetMaxScanTime, MBi_GetTgid, MBi_GetAttribute, MBi_IsStarted and
 * MBi_CheckWmErrcode. The rest are an older SDK's versions of their
 * counterparts; each function's comment says which and what differs, and they
 * keep their address names until a reference of this SDK's age turns up.
 *
 * This SDK is older than the reconstruction's. MB_Init takes the GGID in two
 * halves, resolves no MB_TGID_AUTO and sets no lifetimes or power-save flag,
 * the parent side has no is_started_ex
 * path, no MBi_OnInitializeDone or MBi_EndCommon and no task thread, and
 * MBi_SendMP pads short frames to a minimum size.
 */
#include "types.h"
#include "CP15.h"
#include "IRQ.h"
#include "WmWork.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "private/wm_views.h"

/* The SDK built this file as C, where the GameInfo copy in
 * RecordScannedParent is an inline 0x20-pass halfword loop. As C++ it is an
 * implicit operator=, which mwccarm calls out of line unless always_inline is
 * on. mwccarm generates code for the file's bodies after it has read all of
 * them, so the pragma has to be in force at the end of the file: scoped with
 * push/pop around RecordScannedParent and its caller, it changes nothing. */
#pragma always_inline on

extern "C" {

/* sDmaNo, sWmWork, mbc (MB_Parent.cpp's pPwork) and sScanParam. */
extern u16 data_020a9db0;
extern char *data_020a9db4;
extern char *data_020a9db8;
extern char data_020a9dc0[];
/* The smallest frame a parent sends (6), the SDK's MB_COMM_P_SENDLEN_MIN
 * side of MBi_SendMP. */
extern int data_0209a088;
/* MB_Init's cursor over the parent name, a static in the SDK's MB_Init. */
extern u16 *data_0209a084;

/* Typed through a cast: the other declarations of data_020a9db8 spell it as
 * a byte pointer (MB_Parent.cpp's MB_WORK). */
#define MB_WORK ((MbParentWork *)data_020a9db8)

/* WMScanParam (data_020a9dc0), WM_StartScan's argument; changeScanChannel steps
 * its channel through the allowed-channel mask. */
typedef struct {
    WmBssDesc *scanBuf; /* 0x0 */
    u16 channel;        /* 0x4 */
    u16 maxChannelTime; /* 0x6 */
    u16 bssid[3];       /* 0x8: SDK u8[6]; all 0xffff = any parent */
} WmScanParam;

#define SCAN_PARAM ((WmScanParam *)data_020a9dc0)

/* NitroSDK WM callback arguments. Every one starts with WMCallback's
 * apiid/errcode; the callbacks pick the shape by apiid. */
typedef struct WmCallback {
    u16 apiid;   /* 0x00 */
    u16 errcode; /* 0x02 */
} WmCallback;

/* Leading fields of WMStartMPCallback (apiid 0xe). */
typedef struct WmStartMPCallback {
    u16 apiid;   /* 0x00 */
    u16 errcode; /* 0x02 */
    u16 state;   /* 0x04 -- 0xa MP_START, 0xb MPEND_IND */
    u8 reserved[2];
    void *recvBuf; /* 0x08 -- the received MP data, passed on for MPEND_IND */
} WmStartMPCallback;

/* Leading fields of WMIndCallback (apiid 0x80). */
typedef struct WmIndCallback {
    u16 apiid;   /* 0x00 */
    u16 errcode; /* 0x02 */
    u16 state;   /* 0x04 */
} WmIndCallback;

/* Leading fields of WMPortRecvCallback. */
typedef struct WmPortRecvCallback {
    u16 apiid;   /* 0x0 */
    u16 errcode; /* 0x2 */
    u16 state;   /* 0x4 -- 7 CONNECTED, 9 DISCONNECTED, 0x15 PORT_RECV */
} WmPortRecvCallback;

/* The 0x80-byte WMGameInfo, copied as one struct. */
typedef struct {
    u16 w[0x40];
} GameInfo;

/* The WM API (WM_StartScan, WM_StartConnect, WM_StartMP, WM_SetLifeTime,
 * WM_SetParentParameter, WM_SetBeaconIndication and the rest). The callbacks
 * are WMCallbackFunc pointers in the SDK's headers; the definitions take them
 * as void *, the type of the callback tables they are stored in. Only
 * WM_SetMPDataToPort's definition still takes its callback as an int. */
extern int WM_Initialize(int buf, void *callback, int dma);
extern int WM_SetIndCallback(void *callback);
extern int WM_Reset(void *callback);
extern int WM_End(void *callback);
extern int WM_ReadStatus(int *status);
extern int WM_StartParent(void *callback);
extern int WM_SetPortCallback(int port, void *callback, void *arg);
extern u16 WM_GetAllowedChannel(void);
extern int WM_SetLifeTime(void *callback, int tableNo, int camLife, int frameLife);
extern int WM_SetParentParameter(void *callback, WmParentParam *parentParam);
extern int WM_SetBeaconIndication(void *callback, int flag);
extern int WM_StartScan(void *callback, char *scanParam);
extern int WM_StartConnect(void *callback, int bssDesc, void *ssid);
extern int WM_StartMPEx(void *callback, int recvBuf, u16 recvBufSize, int sendBuf, u16 sendBufSize,
                        u16 mpFreq, u16 defaultRetryCount, int minPollBmpMode, int singlePacketMode,
                        int fixFreqMode, int ignoreFatalError);
/* It returns WM_SetMPDataToPortEx's WM result. */
extern int WM_SetMPDataToPort(int callback, int data, int size, int aidBitmap, u16 port, u16 prio);

extern void OS_GetMacAddress(unsigned char *macAddress);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiCopyHalf(const void *src, void *dst, int count);
extern void DMASyncHalfTransfer(unsigned int ch, void *src, void *dst, unsigned int n);
// local extern: MI_Memory.c defines these two as assembly stubs.
extern void CpuCopy8(const void *src, void *dst, u32 size);
extern void func_0205a588(void *dst, u8 val, u32 size);

extern void MB_CommSetParentStateCallback(u32 callback);
extern void MB_InitSendGameInfoStatus(void);
extern void MBi_CommParentCallback(int type, char *arg);

void MBi_CheckWmErrcode(int apiid, int errcode);
int func_02068a74(void);
void MBi_SetMaxScanTime(u16 time);

/* changeScanChannel: step the scan's channel to the next one the allowed mask
 * (WM_GetAllowedChannel) has, counting 1..16 and wrapping. Returns 0 when no
 * channel is allowed. When the current channel is the only one allowed it is
 * left as it is. This is the HeartGold fork's changeScanChannel as written:
 * dropped in, it compiles to these bytes. It steps WMScanParam.channel as a
 * number and tests bit channel-1. ntrtwl's is the same body. The BW2
 * reconstruction's steps a WMScanExParam.channelList bitmask instead. */
int changeScanChannel(WmScanParam *param) {
    int allowed = WM_GetAllowedChannel();
    u16 channel, i;
    if (allowed == 0) {
        return 0;
    }
    channel = param->channel;
    i = 0;
    do {
        if ((allowed & (1 << (channel - 1))) && param->channel != channel) {
            param->channel = channel;
            break;
        }
        i++;
        channel = channel == 16 ? 1 : channel + 1;
    } while (i < 16);
    return 1;
}

/* Whether frames may go out: MP runs, nothing is on its way, no end is asked
 * for and a child is connected. */
int MBi_IsSendEnabled(void) {
    int ready = 0;
    int idle = 0;
    int running = 0;

    if (WM_WORK->mpStarted == 1 && WM_WORK->mpSending == 0) {
        running = 1;
    }
    if (running && WM_WORK->resetting == 0) {
        idle = 1;
    }
    if (idle && WM_WORK->childBitmap != 0) {
        ready = 1;
    }
    return ready;
}

/* MBi_ParentCallback: the parent's callback for the manager's replies.
 * Initialize, then WM_SetLifeTime, WM_SetParentParameter,
 * WM_SetBeaconIndication and WM_StartParent; on a child's CONNECTED it starts
 * MP once. Lead: the reconstruction's version calls MBi_OnInitializeDone and
 * MBi_EndCommon, starts the parent with WMi_StartParentEx and keeps the child
 * bitmap from each callback's aid; this one calls the WM functions itself,
 * refreshes the bitmap through WM_ReadStatus and decides on starting MP from
 * the status (+0x10) instead of mpStarted; mStartMpBusy is the SDK's
 * start_mp_busy. */
void func_02069994(char *arg0) {
    WmCallback *cb = (WmCallback *)arg0;

    switch (cb->apiid) {
    case 0x0:
        if (cb->errcode == 0) {
            MBi_CheckWmErrcode(0x83, WM_ReadStatus((int *)&WM_WORK->status));
            MBi_CheckWmErrcode(0x80, WM_SetIndCallback((void *)func_02069994));
            MBi_CheckWmErrcode(0x1D, WM_SetLifeTime((void *)func_02069994, 0xFFFF, 0x28, 5));
            return;
        }
        WM_WORK->userCallback(0x100, arg0);
        return;

    case 0x1D:
        if (cb->errcode == 0) {
            MBi_CheckWmErrcode(7,
                               WM_SetParentParameter((void *)func_02069994, &WM_WORK->parentParam));
            return;
        }
        WM_WORK->userCallback(0x100, arg0);
        return;

    case 0x7:
        WM_WORK->userCallback(0x15, arg0);
        MBi_CheckWmErrcode(0x19, WM_SetBeaconIndication((void *)func_02069994, 1));
        return;

    case 0x19:
        if (cb->errcode == 0) {
            MBi_CheckWmErrcode(8, WM_StartParent((void *)func_02069994));
            return;
        }
        WM_WORK->userCallback(0x100, arg0);
        return;

    case 0x8:
        if (cb->errcode == 0) {
            switch (((WmStartParentCallback *)arg0)->state) {
            case 0:
                return;
            case 7:
                MBi_CheckWmErrcode(0x83, WM_ReadStatus((int *)&WM_WORK->status));
                WM_WORK->childBitmap = WM_WORK->status.child_bitmap;
                WM_WORK->userCallback(0, arg0);
                /* WmStatus +0x10: a BOOL WM_ReadStatus fills in; which SDK
                   field it is isn't pinned down here, so it stays raw */
                if (*(int *)(data_020a9db4 + 0x1428) == 0) {
                    MbParentWork *mb = MB_WORK;
                    if (mb->mStartMpBusy == 0) {
                        int v = 1;
                        mb->mStartMpBusy = 1;
                        if (WM_WORK->mpContinuous != 0) {
                            v = 0;
                        }
                        MBi_CheckWmErrcode(
                            0xE, WM_StartMPEx((void *)func_02069994, (int)WM_WORK->recvBuf,
                                              WM_WORK->recvBufSize, (int)WM_WORK->sendBuf,
                                              WM_WORK->sendBufSize, (u16)v, 0, 0, 0, 1, 1));
                        return;
                    }
                }
                if (MBi_IsSendEnabled() == 0) {
                    return;
                }
                WM_WORK->userCallback(0x19, 0);
                return;
            case 9:
                MBi_CheckWmErrcode(0x83, WM_ReadStatus((int *)&WM_WORK->status));
                WM_WORK->childBitmap = WM_WORK->status.child_bitmap;
                WM_WORK->userCallback(1, arg0);
                return;
            case 2:
                WM_WORK->userCallback(0x1C, arg0);
                return;
            default:
                WM_WORK->userCallback(0x100, arg0);
                return;
            }
        } else {
            WM_WORK->userCallback(0x100, arg0);
            return;
        }

    case 0xE:
        MB_WORK->mStartMpBusy = 0;
        switch (((WmStartMPCallback *)arg0)->state) {
        case 0xA:
            WM_WORK->mpStarted = 1;
            WM_WORK->userCallback(0x19, 0);
            return;
        case 0xB:
            WM_WORK->userCallback(3, ((WmStartMPCallback *)arg0)->recvBuf);
            return;
        default:
            WM_WORK->userCallback(0x100, arg0);
            return;
        }

    case 0xF:
        WM_WORK->mpSending = 0;
        {
            u16 t = cb->errcode;
            if (t == 0) {
                WM_WORK->userCallback(2, arg0);
                WM_WORK->userCallback(0x19, 0);
                return;
            }
            if (t == 0xA) {
                WM_WORK->userCallback(0x2A, arg0);
                return;
            }
            WM_WORK->userCallback(0x13, arg0);
            WM_WORK->userCallback(0x19, 0);
            return;
        }

    case 0x1:
        if (cb->errcode == 0) {
            MBi_CheckWmErrcode(2, WM_End((void *)func_02069994));
            return;
        }
        WM_WORK->resetting = 0;
        WM_WORK->userCallback(0x100, arg0);
        return;

    case 0x2:
        if (cb->errcode == 0) {
            WM_WORK->initialized = 0;
            MB_WORK->mIsMbInitialized = 0;
            WM_WORK->userCallback(0x11, arg0);
            return;
        }
        WM_WORK->resetting = 0;
        WM_WORK->userCallback(0x100, arg0);
        return;

    case 0xD:
        if (cb->errcode != 0) {
            return;
        }
        MBi_CheckWmErrcode(0x83, WM_ReadStatus((int *)&WM_WORK->status));
        WM_WORK->childBitmap = WM_WORK->status.child_bitmap;
        return;

    case 0x15:
        if (MBi_IsSendEnabled() == 0) {
            return;
        }
        WM_WORK->userCallback(0x19, 0);
        return;

    case 0x80:
        switch (((WmIndCallback *)arg0)->state - 0x10) {
        case 0:
            WM_WORK->userCallback(0x1D, arg0);
            return;
        case 1:
            WM_WORK->userCallback(0x1F, arg0);
            return;
        case 2:
            WM_WORK->userCallback(0x20, arg0);
            return;
        case 3:
            WM_WORK->userCallback(0x21, arg0);
            return;
        }
        break;

    default:
        WM_WORK->userCallback(0x100, arg0);
        return;
    }
}

/* MBi_ChildPortCallback: the port-1 receive callback func_020690b0 installs
 * through WM_SetPortCallback. A successful PORT_RECV goes to userCallback as
 * event 9. Lead: the reconstruction's switch also lists states 0x19 and 0x1a
 * (DISCONNECTED_FROM_MYSELF), and with those cases this does not compile to
 * the ROM's bytes (0xdc, not 0x7c). */
void func_02069918(void *arg) {
    WmPortRecvCallback *cb = (WmPortRecvCallback *)arg;

    if (cb->errcode == 0) {
        switch (cb->state) {
        case 7:
            break;
        case 9:
            break;
        case 0x15:
            WM_WORK->userCallback(9, arg);
            break;
        }
    }
}

/* PARENT_FOUND: refresh the record of a parent already seen (same MAC) or
 * add one, and point lastParentIndex at it. The record's first 0xc0 bytes are
 * the WMBssDesc DMA'd from the scan buffer, and +0xc0 is a copy of the
 * 0xb8-byte WMStartScanCallback. */
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
            CP15::InvalidateDataCache((u32)&w->parents[i], 0xc0);
            DMASyncHalfTransfer(data_020a9db0, &w->scanBuf, &w->parents[i], 0xc0);
            w->lastParentIndex = i;
            return;
        }
    }
    if (i < 0x10) {
        w->parentCount = i + 1;
        MultiCopyHalf(arg0, &tbl[i].scan, 0xb8);
        CP15::InvalidateDataCache((u32)&w->parents[i], 0xc0);
        DMASyncHalfTransfer(data_020a9db0, &w->scanBuf, &w->parents[i], 0xc0);
        w->lastParentIndex = i;
    }
}

/* MBi_ChildCallback: the child side's callback. Initialize, then
 * WM_SetLifeTime and WM_StartScan; each PARENT_FOUND lands in
 * WmWork.parents[] (RecordScannedParent), and the scan goes on, a channel at
 * a time while scanNextChannel is set, until scanContinue is cleared. Then
 * WM_StartConnect and, on CONNECTED, the port-1 callback and WM_StartMP.
 * While resetting is set only the RESET and END results are taken. Lead: the
 * reconstruction's version scans with WM_StartScanEx and filters parents by
 * GGID; this one uses WM_StartScan and keeps every parent it finds. */
void func_020690b0(char *arg0) {
    WmWork *const w = (WmWork *)data_020a9db4;

    if (w->resetting != 0) {
        if (*(u16 *)arg0 == 1) {
            if (*(u16 *)(arg0 + 2) == 0) {
                MBi_CheckWmErrcode(2, WM_End((void *)func_020690b0));
                return;
            }
            w->resetting = 0;
            w->userCallback(0x100, arg0);
            return;
        }
        if (*(u16 *)arg0 != 2)
            return;
        if (*(u16 *)(arg0 + 2) == 0) {
            w->initialized = 0;
            MB_WORK->mIsMbInitialized = 0;
            w->userCallback(0x11, arg0);
            return;
        }
        w->resetting = 0;
        w->userCallback(0x100, arg0);
        return;
    }

    switch (*(u16 *)arg0) {
    case 0:
        if (*(u16 *)(arg0 + 2) == 0) {
            w->userCallback(0x15, arg0);
            MBi_CheckWmErrcode(0x83, WM_ReadStatus((int *)&w->status));
            MBi_CheckWmErrcode(0x1d, WM_SetLifeTime((void *)func_020690b0, 0xffff, 0x28, 5));
            return;
        }
        w->userCallback(0x100, arg0);
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
            MBi_CheckWmErrcode(0xa, WM_StartScan((void *)func_020690b0, data_020a9dc0));
            return;
        }
        w->userCallback(0x100, arg0);
        return;

    case 0xa:
        if (*(u16 *)(arg0 + 2) == 0) {
            switch (*(u16 *)(arg0 + 8)) {
            case 3:
                return;
            case 5:
                RecordScannedParent(w, arg0);
                w->userCallback(4, arg0);
                if (w->scanContinue == 0)
                    return;
                if (w->scanNextChannel != 0 && changeScanChannel(SCAN_PARAM) == 0)
                    MBi_CheckWmErrcode(2, func_02068a74());
                MBi_CheckWmErrcode(0xa, WM_StartScan((void *)func_020690b0, data_020a9dc0));
                return;
            case 4:
                w->userCallback(5, arg0);
                if (w->scanContinue == 0)
                    return;
                if (w->scanNextChannel != 0 && changeScanChannel(SCAN_PARAM) == 0)
                    MBi_CheckWmErrcode(2, func_02068a74());
                MBi_CheckWmErrcode(0xa, WM_StartScan((void *)func_020690b0, data_020a9dc0));
                return;
            default:
                w->userCallback(0x100, arg0);
                return;
            }
        } else {
            w->userCallback(0x100, arg0);
        }
        return;

    case 0xb:
        if (*(u16 *)(arg0 + 2) == 0) {
            MBi_CheckWmErrcode(0xc, WM_StartConnect((void *)func_020690b0, w->connectBssDesc, 0));
            return;
        }
        w->userCallback(0x100, arg0);
        return;

    case 0xc:
        if (*(u16 *)(arg0 + 2) == 0) {
            switch (*(u16 *)(arg0 + 8)) {
            case 6:
                return;
            case 7:
                w->aid = ((WmStartConnectCallback *)arg0)->aid;
                w->userCallback(6, arg0);
                w->childBitmap = 1;
                if (WM_SetPortCallback(1, (void *)func_02069918, 0) != 0)
                    return;
                MBi_CheckWmErrcode(
                    0xe, WM_StartMPEx((void *)func_020690b0, (int)w->recvBuf, w->recvBufSize,
                                      (int)w->sendBuf, w->sendBufSize,
                                      (u16)(w->mpContinuous == 0 ? 1 : 0), 0, 0, 0, 1, 1));
                return;
            case 9:
                w->userCallback(0xa, arg0);
                w->childBitmap = 0;
                return;
            default:
                w->userCallback(0x100, arg0);
                return;
            }
        } else {
            w->parentCount = 0;
            w->userCallback(0xb, arg0);
        }
        return;

    case 0xe:
        switch (*(u16 *)(arg0 + 4)) {
        case 0xa:
            w->mpStarted = 1;
            if (MBi_IsSendEnabled() == 0)
                return;
            w->userCallback(0x19, 0);
            return;
        case 0xc:
            return;
        case 0xd:
            return;
        default:
            w->userCallback(0x100, arg0);
            return;
        }

    case 0xf:
        w->mpSending = 0;
        if (*(u16 *)(arg0 + 2) == 0)
            w->userCallback(8, arg0);
        else if (*(u16 *)(arg0 + 2) == 9)
            w->userCallback(0x29, arg0);
        else
            w->userCallback(0x12, arg0);
        w->userCallback(0x19, 0);
        return;

    case 0x15:
        if (MBi_IsSendEnabled() == 0)
            return;
        w->userCallback(0x19, 0);
        return;

    case 0x80:
        return;

    default:
        w->userCallback(0x100, arg0);
        return;
    }
}

/* A number from 0 to 19 from the console's MAC address and the VBlank count at
 * 0x027ffc3c (OS_GetVBlankCount); MB_Init adds it to the beacon period. */
int MBi_GetBeaconPeriodDispersion(void) {
    u8 mac[6];
    u32 sum;
    int i;

    OS_GetMacAddress(mac);
    for (i = 0, sum = 0; i < 6; i++) {
        sum += mac[i];
    }
    return (sum + *(int *)0x27ffc3c) * 7 % 20;
}

/* MB_Init: carve the WmWork and mbc out of the caller's buffer (each 32-byte
 * aligned), clear them, copy the user name and the parent name, store the
 * user info and set the parent parameters. Refused (2) while
 * mIsMbInitialized (the SDK's isMbInitialized) is set. Lead: the reconstruction's MB_Init
 * takes (work, user, ggid, tgid, dma), resolves MB_TGID_AUTO and sets the
 * lifetimes and the power-save flag; this one takes the GGID as two halves
 * (a2, a3) and does none of that. */
int func_02068e4c(int a0, u16 *a1, int a2, int a3, u16 a4, int a5) {
    volatile unsigned short fill_s;
    volatile int fill_i;
    /* base1 (the WmWork) and base2 are byte pointers, not ints: every WmWork
     store below (parentMaxSize, sendBufSize, sendBuf, ...) is add-high /
     small-offset on the cartridge (add r0,r7,#0x1e00 / strh r3,[r0,#0x18]),
     which mwcc only emits for pointer arithmetic. With int bases each offset
     is materialized from the literal pool and stored through a register
     index, 15 words longer. */
    char *base1;
    char *base2;
    int irq;
    if ((data_020a9db8 != 0) && (MB_WORK->mIsMbInitialized != 0)) {
        return 2;
    }
    base1 = (char *)((a0 + 0x1f) & (~0x1f));
    base2 = (char *)(((int)base1 + sizeof(WmWork) + 0x1f) & (~0x1f));
    irq = IRQ::Disable();
    fill_i = 0;
    data_020a9db0 = (u16)a5;
    data_020a9db4 = base1;
    data_020a9db8 = base2;
    MultiStore_Int(fill_i, (int *)base1, sizeof(WmWork));
    fill_s = 0;
    MultiStore16(fill_s, base2, 0x1340);
    /* 0x1e30 and 0x1e38 sit in WmWork.pad_1e2e: nothing else in this file
     reads them back (and the two copies overlap), so they stay raw */
    {
        int i = 0;
        u16 *dst = (u16 *)(base1 + 0x1e30);
        u16 *rp = a1;
        u16 v;
        do {
            v = *((rp + i) + 1);
            if (v == 0) {
                break;
            }
            i++;
            *(dst++) = v;
        } while (i < 0xa);
    }
    {
        u16 *dst = (u16 *)(base1 + 0x1e38);
        int j = 0;
        do {
            u16 *ptr = data_0209a084;
            if ((*ptr) == 0) {
                break;
            }
            data_0209a084 = ptr + 1;
            j++;
            *(dst++) = *ptr;
        } while (j < 0x10);
    }
    CpuCopy8(a1, base2 + 0x1300, 0x16);
    ((WmWork *)base1)->parentMaxSize = 0x100;
    ((WmWork *)base1)->childMaxSize = 8;
    ((WmWork *)base1)->sendBufSize = 0;
    ((WmWork *)base1)->recvBufSize = 0;
    ((WmWork *)base1)->mpContinuous = 1;
    ((WmWork *)base1)->sendBuf = base2;
    ((WmWork *)base1)->recvBuf = base2 + 0x400;
    {
        WmParentParam *hdr = &((WmWork *)base1)->parentParam;
        hdr->CS_Flag = 1;
        hdr->KS_Flag = 0;
        hdr->ggid[0] = (short)a2;
        hdr->ggid[1] = (short)a3;
        hdr->tgid = (short)a4;
        hdr->entryFlag = 1;
        hdr->multiBootFlag = 0;
        hdr->beaconPeriod = (u16)(MBi_GetBeaconPeriodDispersion() + 0xc8);
        hdr->maxEntry = 0xf;
    }
    ((WmWork *)base1)->mpSending = 0;
    ((WmWork *)base1)->initialized = 0;
    ((MbParentWork *)base2)->mStartMpBusy = 0;
    IRQ::Restore(irq);
    ((MbParentWork *)base2)->mIsMbInitialized = 1;
    return 0;
}

/* MBi_IsCommSizeValid: the size check func_02068dc8 runs. Lead: the
 * reconstruction's checks the parent size (0xe4..0x1fe), the child size
 * (8..0x10) and the buffer they need; this one accepts everything. */
int func_02068e44(int parentMaxSize, int childMaxSize, int maxEntry) {
    return 1;
}

/* MB_SetParentCommParam: set the MP sizes and the parent's maxEntry. Refused
 * (0) once the wireless layer is initialized, or if func_02068e44 rejects
 * them. The game's link setup func_0203e20c passes (0x1fe, 8, 3). Lead: the
 * reconstruction's takes (sendSize, maxChildren), fixes the child size at 8
 * and disables interrupts around it; this one takes all three sizes and
 * disables nothing. */
int func_02068dc8(int parentMaxSize, int childMaxSize, int maxEntry) {
    if (WM_WORK->initialized)
        return 0;
    if (func_02068e44(parentMaxSize, childMaxSize, maxEntry) == 0)
        return 0;
    WM_WORK->parentParam.maxEntry = maxEntry;
    WM_WORK->parentMaxSize = parentMaxSize;
    WM_WORK->childMaxSize = childMaxSize;
    return 1;
}

/* MBi_StartCommon: clear the MP state, set the scan time and start the
 * manager (WM_Initialize until it is not WM_DISABLE), 8 if that fails. Lead:
 * the reconstruction's sets the scan time to 30, not 10, and has an
 * is_started_ex path that skips WM_Initialize. */
int func_02068d08(void) {
    WmWork *base;
    int ret;
    base = WM_WORK;
    base->mpStarted = 0;
    WM_WORK->childBitmap = 0;
    WM_WORK->resetting = 0;
    *(short *)(data_020a9db4 + 0x1e48) = 0; /* in WmWork.pad_1e2e: no reader here */
    MBi_SetMaxScanTime(0xa);
    do {
        ret = WM_Initialize((int)WM_WORK->wmSysBuf, (void *)WM_WORK->stateCallback, data_020a9db0);
    } while (ret == 4);
    if (ret != 2)
        return 8;
    WM_SetIndCallback((void *)WM_WORK->stateCallback);
    WM_WORK->initialized = 1;
    return 0;
}

/* MBi_StartParentCore with MB_StartParent: record the channel, clear the
 * parent work from mChildUser on (keeping the parent callback across the
 * clear), size the blocks and the child request pieces from the MP packet
 * sizes, mark every child as in no file, clear the file table and the
 * pending answers, then hand WM the parent callbacks and buffer sizes and
 * start. Lead: the reconstruction's sets mbc->is_started_ex in a separate
 * MB_StartParent, sizes the request pieces through MBi_SetChildMPMaxSize and
 * reads PXI_IsCallbackReady for useWvrFlag at the end. */
int func_02068ae8(int param0) {
    u32 saved;
    volatile u16 sp0;
    volatile u16 sp2;
    u32 cb;
    int i;
    int result;

    saved = IRQ::Disable();

    WM_WORK->parentParam.channel = param0;

    cb = MB_WORK->mParentCallback;

    sp0 = 0;
    MultiStore16(sp0, (char *)MB_WORK->mChildUser, 0x60a0);

    MB_CommSetParentStateCallback(cb);

    MB_WORK->mBlockSize = WM_WORK->parentMaxSize - 6;
    MB_WORK->mReqPieceSize = WM_WORK->childMaxSize - 2;
    MB_WORK->mReqPieceCount = 30 / (WM_WORK->childMaxSize - 2);

    for (i = 0; i < 0xf; i++) {
        MB_WORK->mChildState[i] = 0;
        MB_WORK->mChildFile[i] = -1;
    }
    MB_WORK->mFileCount = 0;

    sp2 = 0;
    MultiStore16(sp2, (char *)MB_WORK->mFiles, 0x5c40);

    func_0205a588(MB_WORK->mChildResponse, 0, 0x1e);

    WM_WORK->mode = 1;
    WM_WORK->userCallback = (void (*)(int, void *))MBi_CommParentCallback;
    WM_WORK->stateCallback = func_02069994;

    WM_WORK->parentParam.parentMaxSize = WM_WORK->parentMaxSize;
    WM_WORK->sendBufSize = (WM_WORK->parentParam.parentMaxSize + 0x23) & ~0x1f;

    WM_WORK->parentParam.childMaxSize = WM_WORK->childMaxSize;
    WM_WORK->recvBufSize = (((WM_WORK->parentParam.childMaxSize + 0xe) * 0xf + 0x29) & ~0x1f) << 1;

    MB_InitSendGameInfoStatus();
    result = func_02068d08();

    IRQ::Restore(saved);

    return result;
}

/* MBi_CommEnd with MBi_CallReset: unless a reset is already under way, stop
 * the scan, mark the reset and call WM_Reset; 0 on OPERATING, 1 if a reset
 * was already running. Lead: the reconstruction's MBi_CommEnd disables
 * interrupts, calls MBi_EndCommon when the manager never started and resets
 * through the task thread when there is one; MBi_CallReset also checks the
 * WM result. */
int func_02068a74(void) {
    WmWork *p = WM_WORK;
    if (p->resetting == 0) {
        int r;
        p->scanContinue = 0;
        /* through the global again: the ROM reloads data_020a9db4 here */
        WM_WORK->resetting = 1;
        r = WM_Reset((void *)WM_WORK->stateCallback);
        if (r == 2)
            return 0;
        return r;
    }
    return 1;
}

/* MB_End: func_02068a74 with interrupts disabled. Lead: the reconstruction's
 * also calls OS_Terminate when MB_StartParent was not used. */
void func_02068a54(void) {
    u32 saved = IRQ::Disable();
    func_02068a74();
    IRQ::Restore(saved);
}

/* MBi_SetMPData: send on port 1 at priority 3 through WM_SetMPDataToPort.
 * The fourth parameter is unused. Lead: the reconstruction's calls
 * WM_SetMPDataToPortEx and passes the result to MBi_CheckWmErrcode. */
int func_02068a28(int callback, int data, int size, int unused, u16 aidBitmap) {
    return WM_SetMPDataToPort(callback, data, size, aidBitmap, 1, 3);
}

/* MBi_SendMP: queue one MP send. Fails (1) until MP has started or while a
 * reset is under way. A parent (mode 1) sends through stateCallback, a child
 * (mode 2) through func_020690b0. On WM's OPERATING (2) it sets mpSending and
 * returns 0; any other code is returned as it is. Lead: the reconstruction's
 * has the same two cases, but this parent case first pads the size up to
 * data_0209a088 (6). */
int func_020688fc(int arg0, int arg1, int arg2) {
    WmWork *hdr = WM_WORK;
    int a = (u16)arg1;
    u16 b = arg2;
    int ret;
    int u;
    if ((hdr->mpStarted == 0) || (hdr->resetting == 1)) {
        return 1;
    }
    switch (hdr->mode) {
    case 1:
        if (a < data_0209a088) {
            a = (u16)data_0209a088;
        }
        if (hdr->mpContinuous == 0) {
            u = 1000;
        } else {
            u = 0;
        }
        ret = func_02068a28((int)hdr->stateCallback, arg0, a, (u16)u, b);
        if (ret == 2) {
            WM_WORK->mpSending = 1;
        }
        if (ret == 2) {
            ret = 0;
        }
        return ret;

    case 2:
        ret = func_02068a28((int)func_020690b0, arg0, a, 0, b);
        if (ret == 2) {
            WM_WORK->mpSending = 1;
        }
        if (ret == 2) {
            ret = 0;
        }
        return ret;

    default:
        return 1;
    }
}

/* parentParam.ggid[0]. Lead: the reconstruction's MBi_GetGgid returns the
 * whole u32; this SDK keeps the GGID as two u16s (WmParentParam) and has a
 * getter for each half, which the beacon senders pass to WM_SetGameInfo. */
int func_020688e4(void) {
    return WM_WORK->parentParam.ggid[0];
}

/* parentParam.ggid[1], the other half (see func_020688e4). */
int func_020688cc(void) {
    return WM_WORK->parentParam.ggid[1];
}

int MBi_GetTgid(void) {
    return WM_WORK->parentParam.tgid;
}

/* The beacon attribute byte for WM_SetGameInfo: entryFlag 1, multiBootFlag
 * 2, KS_Flag 4, CS_Flag 8 (the SDK's WM_ATTR_FLAG_* bits). The beacon senders
 * OR in 2 again. */
int MBi_GetAttribute(void) {
    int a = WM_WORK->parentParam.multiBootFlag ? 2 : 0;
    int b = WM_WORK->parentParam.entryFlag ? 1 : 0;
    int c = WM_WORK->parentParam.KS_Flag ? 4 : 0;
    int d = WM_WORK->parentParam.CS_Flag ? 8 : 0;
    return (d | (c | (b | a))) & 0xff;
}

void MBi_SetMaxScanTime(u16 time) {
    SCAN_PARAM->maxChannelTime = time;
}

/* WmWork.parentMaxSize, the parent's MP send size (set by func_02068dc8).
 * MBi_CommParentCallback uses it to size the request record it clears for a
 * child. Lead: the reconstruction has no getter for p_sendlen. */
int func_0206881c(void) {
    return WM_WORK->parentMaxSize;
}

/* Whether the wireless layer is up: WmWork.initialized == 1, set after
 * WM_Initialize. func_02067928 refuses to register a file before then. */
int MBi_IsStarted(void) {
    return WM_WORK->initialized == 1;
}

/* Report a failed WM call: {apiid, errcode} goes to WmWork.userCallback as
 * event 0xff, MBi_CommParentCallback's failed-WM-call code. A call that
 * succeeded (0) or is still running (2, OPERATING) is dropped. */
void MBi_CheckWmErrcode(int apiid, int errcode) {
    struct {
        s16 apiid;
        s16 errcode;
    } p;
    if (errcode == 2)
        return;
    if (errcode == 0)
        return;
    p.apiid = (s16)apiid;
    p.errcode = (s16)errcode;
    WM_WORK->userCallback(0xff, &p);
}
}
