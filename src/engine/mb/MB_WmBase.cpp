//cpp
/* The multiboot library's use of the wireless manager (the Nitro SDK's
 * mb_wm_base.c), arm9 .text 0x0206879c..0x0206a318.
 *
 * The parent side sets up the WmWork (data_020a9db4, the BW2 reconstruction's sWmWork;
 * ntrtwl's p_mbi_param) in
 * MB_Init, starts the manager in MB_StartParent and MBi_StartCommon, and
 * answers its replies in MBi_ParentCallback. The child side scans and connects in
 * MBi_ChildCallback. MBi_SendMP sends one MP frame for either side, and
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
 * MBi_CheckWmErrcode. Twelve more are an older SDK's versions of their
 * counterparts and take those names too, since an SDK function keeps its name
 * across versions; each one's comment says what differs. Where this SDK has
 * one function for the reconstruction's two, it takes the name its callers
 * use: MB_StartParent (with MBi_StartParentCore inside) and MBi_CommEnd (with
 * MBi_CallReset). Three getters have no counterpart and keep address names.
 *
 * This SDK is older than the reconstruction's. MB_Init takes the GGID in two
 * halves, resolves no MB_TGID_AUTO and sets no lifetimes or power-save flag,
 * the parent side has no is_started_ex
 * path, no MBi_OnInitializeDone or MBi_EndCommon and no task thread, and
 * MBi_SendMP pads short frames to a minimum size.
 */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/os/interrupt_vblank.h"
#include "nitro/wm/wm_api.h"
#include "WmWork.h"
#include "private/arm9_a9db8_mb_parent.h"
#include "private/mb_views.h"
#include "private/mb_wm_base.h"
#include "private/wm_views.h"
#include "nitro/mi/memory.h"
#include "nitro/mi/dma.h"

/* The SDK built this file as C, where the GameInfo copy in
 * RecordScannedParent is an inline 0x20-pass halfword loop. As C++ it is an
 * implicit operator=, which mwccarm calls out of line unless always_inline is
 * on. mwccarm generates code for the file's bodies after it has read all of
 * them, so the pragma has to be in force at the end of the file: scoped with
 * push/pop around RecordScannedParent and its caller, it changes nothing. */
#pragma always_inline on

/* sDmaNo, sWmWork and sScanParam; mbc and pPwork are mb_parent's
 * (private/arm9_a9db8_mb_parent.h). */
extern u16 data_020a9db0;
extern char *data_020a9db4;
extern WMScanParam data_020a9dc0;
/* The smallest frame a parent sends (6), the SDK's MB_COMM_P_SENDLEN_MIN
 * side of MBi_SendMP. */
extern int data_0209a088;
/* MB_Init's cursor over the parent name, a static in the SDK's MB_Init. */
extern u16 *data_0209a084;

/* nitro/math.h's round-up, which MB_Init uses to align the caller's work
 * buffer to a cache line before carving it. */
#define MATH_ROUNDUP(x, base) (((x) + ((base) - 1)) & ~((base) - 1))

/* The WMScanParam WM_StartScan reads; changeScanChannel steps its channel
 * through the allowed-channel mask. */
#define SCAN_PARAM (&data_020a9dc0)

/* The 0x80-byte WMGameInfo, copied as one struct. */
typedef struct {
    u16 w[0x40];
} GameInfo;
extern "C" {

extern void OS_GetMacAddress(unsigned char *macAddress);

extern void MB_InitSendGameInfoStatus(void);
}

void MBi_CheckWmErrcode(int apiid, int errcode);
int MBi_CommEnd(void);
void MBi_SetMaxScanTime(u16 time);

/* changeScanChannel: step the scan's channel to the next one the allowed mask
 * (WM_GetAllowedChannel) has, counting 1..16 and wrapping. Returns 0 when no
 * channel is allowed. When the current channel is the only one allowed it is
 * left as it is. This is the HeartGold fork's changeScanChannel as written:
 * dropped in, it compiles to these bytes. It steps WMScanParam.channel as a
 * number and tests bit channel-1. ntrtwl's is the same body. The BW2
 * reconstruction's steps a WMScanExParam.channelList bitmask instead. */
int changeScanChannel(WMScanParam *param) {
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
 * the status's mp_flag instead of mpStarted; start_mp_busy is the SDK's
 * start_mp_busy. */
void MBi_ParentCallback(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    switch (cb->apiid) {
    case WM_APIID_INITIALIZE:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            MBi_CheckWmErrcode(WM_APIID_READ_STATUS, WM_ReadStatus(&WM_WORK->status));
            MBi_CheckWmErrcode(WM_APIID_INDICATION, WM_SetIndCallback(MBi_ParentCallback));
            MBi_CheckWmErrcode(WM_APIID_SET_LIFETIME,
                               WM_SetLifeTime(MBi_ParentCallback, 0xFFFF, 0x28, 5));
            return;
        }
        WM_WORK->userCallback(0x100, arg);
        return;

    case WM_APIID_SET_LIFETIME:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            MBi_CheckWmErrcode(WM_APIID_SET_P_PARAM,
                               WM_SetParentParameter(MBi_ParentCallback, &WM_WORK->parentParam));
            return;
        }
        WM_WORK->userCallback(0x100, arg);
        return;

    case WM_APIID_SET_P_PARAM:
        WM_WORK->userCallback(0x15, arg);
        MBi_CheckWmErrcode(WM_APIID_SET_BEACON_IND, WM_SetBeaconIndication(MBi_ParentCallback, 1));
        return;

    case WM_APIID_SET_BEACON_IND:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            MBi_CheckWmErrcode(WM_APIID_START_PARENT, WM_StartParent(MBi_ParentCallback));
            return;
        }
        WM_WORK->userCallback(0x100, arg);
        return;

    case WM_APIID_START_PARENT:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            switch (((WmStartParentCallback *)arg)->state) {
            case WM_STATECODE_PARENT_START:
                return;
            case WM_STATECODE_CONNECTED:
                MBi_CheckWmErrcode(WM_APIID_READ_STATUS, WM_ReadStatus(&WM_WORK->status));
                WM_WORK->childBitmap = WM_WORK->status.child_bitmap;
                WM_WORK->userCallback(0, arg);
                if (WM_WORK->status.mp_flag == 0) {
                    MB_CommPWork *mb = pPwork;
                    if (mb->common.start_mp_busy == 0) {
                        int mpFreq = 1;
                        mb->common.start_mp_busy = 1;
                        if (WM_WORK->mpContinuous != 0) {
                            mpFreq = 0;
                        }
                        MBi_CheckWmErrcode(
                            WM_APIID_START_MP,
                            WM_StartMPEx(MBi_ParentCallback, (u16 *)WM_WORK->recvBuf,
                                         WM_WORK->recvBufSize, (u16 *)WM_WORK->sendBuf,
                                         WM_WORK->sendBufSize, mpFreq, 0, 0, 0, 1, 1));
                        return;
                    }
                }
                if (MBi_IsSendEnabled() == 0) {
                    return;
                }
                WM_WORK->userCallback(0x19, 0);
                return;
            case WM_STATECODE_DISCONNECTED:
                MBi_CheckWmErrcode(WM_APIID_READ_STATUS, WM_ReadStatus(&WM_WORK->status));
                WM_WORK->childBitmap = WM_WORK->status.child_bitmap;
                WM_WORK->userCallback(1, arg);
                return;
            case WM_STATECODE_BEACON_SENT:
                WM_WORK->userCallback(0x1C, arg);
                return;
            default:
                WM_WORK->userCallback(0x100, arg);
                return;
            }
        } else {
            WM_WORK->userCallback(0x100, arg);
            return;
        }

    case WM_APIID_START_MP:
        mbc->start_mp_busy = 0;
        switch (((WMStartMPCallback *)arg)->state) {
        case WM_STATECODE_MP_START:
            WM_WORK->mpStarted = 1;
            WM_WORK->userCallback(0x19, 0);
            return;
        case WM_STATECODE_MPEND_IND:
            WM_WORK->userCallback(3, ((WMStartMPCallback *)arg)->recvBuf);
            return;
        default:
            WM_WORK->userCallback(0x100, arg);
            return;
        }

    case WM_APIID_SET_MP_DATA:
        WM_WORK->mpSending = 0;
        {
            u16 err = cb->errcode;
            if (err == WM_ERRCODE_SUCCESS) {
                WM_WORK->userCallback(2, arg);
                WM_WORK->userCallback(0x19, 0);
                return;
            }
            if (err == WM_ERRCODE_SEND_QUEUE_FULL) {
                WM_WORK->userCallback(0x2A, arg);
                return;
            }
            WM_WORK->userCallback(0x13, arg);
            WM_WORK->userCallback(0x19, 0);
            return;
        }

    case WM_APIID_RESET:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            MBi_CheckWmErrcode(WM_APIID_END, WM_End(MBi_ParentCallback));
            return;
        }
        WM_WORK->resetting = 0;
        WM_WORK->userCallback(0x100, arg);
        return;

    case WM_APIID_END:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            WM_WORK->initialized = 0;
            mbc->isMbInitialized = 0;
            WM_WORK->userCallback(0x11, arg);
            return;
        }
        WM_WORK->resetting = 0;
        WM_WORK->userCallback(0x100, arg);
        return;

    case WM_APIID_DISCONNECT:
        if (cb->errcode != WM_ERRCODE_SUCCESS) {
            return;
        }
        MBi_CheckWmErrcode(WM_APIID_READ_STATUS, WM_ReadStatus(&WM_WORK->status));
        WM_WORK->childBitmap = WM_WORK->status.child_bitmap;
        return;

    case WM_APIID_START_KS:
        if (MBi_IsSendEnabled() == 0) {
            return;
        }
        WM_WORK->userCallback(0x19, 0);
        return;

    case WM_APIID_INDICATION:
        switch (((WMIndCallback *)arg)->state) {
        case WM_STATECODE_BEACON_RECV:
            WM_WORK->userCallback(0x1D, arg);
            return;
        case WM_STATECODE_DISASSOCIATE:
            WM_WORK->userCallback(0x1F, arg);
            return;
        case WM_STATECODE_REASSOCIATE:
            WM_WORK->userCallback(0x20, arg);
            return;
        case WM_STATECODE_AUTHENTICATE:
            WM_WORK->userCallback(0x21, arg);
            return;
        }
        break;

    default:
        WM_WORK->userCallback(0x100, arg);
        return;
    }
}

/* MBi_ChildPortCallback: the port-1 receive callback MBi_ChildCallback installs
 * through WM_SetPortCallback. A successful PORT_RECV goes to userCallback as
 * event 9. Lead: the reconstruction's switch also lists states 0x19 and 0x1a
 * (DISCONNECTED_FROM_MYSELF), and with those cases this does not compile to
 * the ROM's bytes (0xdc, not 0x7c). */
void MBi_ChildPortCallback(void *arg) {
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        switch (cb->state) {
        case WM_STATECODE_CONNECTED:
            break;
        case WM_STATECODE_DISCONNECTED:
            break;
        case WM_STATECODE_PORT_RECV:
            WM_WORK->userCallback(9, arg);
            break;
        }
    }
}

/* PARENT_FOUND: refresh the record of a parent already seen (same MAC) or
 * add one, and point lastParentIndex at it. The record's first 0xc0 bytes are
 * the WMBssDesc DMA'd from the scan buffer, and +0xc0 is a copy of the
 * 0xb8-byte WMStartScanCallback. */
static inline void RecordScannedParent(WmWork *const w, WmStartScanCallback *const cb) {
    WmParentRecord *tbl = w->parents;
    int i;
    for (i = 0; i < w->parentCount; i++) {
        if (cb->macAddress[0] == tbl[i].scan.macAddress[0] &&
            cb->macAddress[1] == tbl[i].scan.macAddress[1] &&
            cb->macAddress[2] == tbl[i].scan.macAddress[2]) {
            tbl[i].scan.gameInfoLength = cb->gameInfoLength;
            *(GameInfo *)tbl[i].scan.gameInfo = *(GameInfo *)cb->gameInfo;
            DC_InvalidateRange(&w->parents[i], 0xc0);
            MI_DmaCopy16(data_020a9db0, &w->scanBuf, &w->parents[i], 0xc0);
            w->lastParentIndex = i;
            return;
        }
    }
    if (i < 0x10) {
        w->parentCount = i + 1;
        MI_CpuCopy16(cb, &tbl[i].scan, 0xb8);
        DC_InvalidateRange(&w->parents[i], 0xc0);
        MI_DmaCopy16(data_020a9db0, &w->scanBuf, &w->parents[i], 0xc0);
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
void MBi_ChildCallback(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    WmWork *const w = (WmWork *)data_020a9db4;

    if (w->resetting != 0) {
        if (cb->apiid == WM_APIID_RESET) {
            if (cb->errcode == WM_ERRCODE_SUCCESS) {
                MBi_CheckWmErrcode(WM_APIID_END, WM_End(MBi_ChildCallback));
                return;
            }
            w->resetting = 0;
            w->userCallback(0x100, arg);
            return;
        }
        if (cb->apiid != WM_APIID_END)
            return;
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            w->initialized = 0;
            mbc->isMbInitialized = 0;
            w->userCallback(0x11, arg);
            return;
        }
        w->resetting = 0;
        w->userCallback(0x100, arg);
        return;
    }

    switch (cb->apiid) {
    case WM_APIID_INITIALIZE:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            w->userCallback(0x15, arg);
            MBi_CheckWmErrcode(WM_APIID_READ_STATUS, WM_ReadStatus(&w->status));
            MBi_CheckWmErrcode(WM_APIID_SET_LIFETIME,
                               WM_SetLifeTime(MBi_ChildCallback, 0xffff, 0x28, 5));
            return;
        }
        w->userCallback(0x100, arg);
        return;

    case WM_APIID_SET_LIFETIME:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
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
            MBi_CheckWmErrcode(WM_APIID_START_SCAN, WM_StartScan(MBi_ChildCallback, SCAN_PARAM));
            return;
        }
        w->userCallback(0x100, arg);
        return;

    case WM_APIID_START_SCAN:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            switch (((WmStartScanCallback *)arg)->state) {
            case WM_STATECODE_SCAN_START:
                return;
            case WM_STATECODE_PARENT_FOUND:
                RecordScannedParent(w, (WmStartScanCallback *)arg);
                w->userCallback(4, arg);
                if (w->scanContinue == 0)
                    return;
                if (w->scanNextChannel != 0 && changeScanChannel(SCAN_PARAM) == 0)
                    MBi_CheckWmErrcode(WM_APIID_END, MBi_CommEnd());
                MBi_CheckWmErrcode(WM_APIID_START_SCAN,
                                   WM_StartScan(MBi_ChildCallback, SCAN_PARAM));
                return;
            case WM_STATECODE_PARENT_NOT_FOUND:
                w->userCallback(5, arg);
                if (w->scanContinue == 0)
                    return;
                if (w->scanNextChannel != 0 && changeScanChannel(SCAN_PARAM) == 0)
                    MBi_CheckWmErrcode(WM_APIID_END, MBi_CommEnd());
                MBi_CheckWmErrcode(WM_APIID_START_SCAN,
                                   WM_StartScan(MBi_ChildCallback, SCAN_PARAM));
                return;
            default:
                w->userCallback(0x100, arg);
                return;
            }
        } else {
            w->userCallback(0x100, arg);
        }
        return;

    case WM_APIID_END_SCAN:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            MBi_CheckWmErrcode(WM_APIID_START_CONNECT,
                               WM_StartConnect(MBi_ChildCallback, w->connectBssDesc, 0));
            return;
        }
        w->userCallback(0x100, arg);
        return;

    case WM_APIID_START_CONNECT:
        if (cb->errcode == WM_ERRCODE_SUCCESS) {
            switch (((WmStartConnectCallback *)arg)->state) {
            case WM_STATECODE_CONNECT_START:
                return;
            case WM_STATECODE_CONNECTED:
                w->aid = ((WmStartConnectCallback *)arg)->aid;
                w->userCallback(6, arg);
                w->childBitmap = 1;
                if (WM_SetPortCallback(1, MBi_ChildPortCallback, 0) != WM_ERRCODE_SUCCESS)
                    return;
                MBi_CheckWmErrcode(WM_APIID_START_MP,
                                   WM_StartMPEx(MBi_ChildCallback, (u16 *)w->recvBuf,
                                                w->recvBufSize, (u16 *)w->sendBuf, w->sendBufSize,
                                                (w->mpContinuous == 0 ? 1 : 0), 0, 0, 0, 1, 1));
                return;
            case WM_STATECODE_DISCONNECTED:
                w->userCallback(0xa, arg);
                w->childBitmap = 0;
                return;
            default:
                w->userCallback(0x100, arg);
                return;
            }
        } else {
            w->parentCount = 0;
            w->userCallback(0xb, arg);
        }
        return;

    case WM_APIID_START_MP:
        switch (((WMStartMPCallback *)arg)->state) {
        case WM_STATECODE_MP_START:
            w->mpStarted = 1;
            if (MBi_IsSendEnabled() == 0)
                return;
            w->userCallback(0x19, 0);
            return;
        case WM_STATECODE_MP_IND:
            return;
        case WM_STATECODE_MPACK_IND:
            return;
        default:
            w->userCallback(0x100, arg);
            return;
        }

    case WM_APIID_SET_MP_DATA:
        w->mpSending = 0;
        if (cb->errcode == WM_ERRCODE_SUCCESS)
            w->userCallback(8, arg);
        else if (cb->errcode == WM_ERRCODE_TIMEOUT)
            w->userCallback(0x29, arg);
        else
            w->userCallback(0x12, arg);
        w->userCallback(0x19, 0);
        return;

    case WM_APIID_START_KS:
        if (MBi_IsSendEnabled() == 0)
            return;
        w->userCallback(0x19, 0);
        return;

    case WM_APIID_INDICATION:
        return;

    default:
        w->userCallback(0x100, arg);
        return;
    }
}

/* A number from 0 to 19 from the console's MAC address and the VBlank count;
 * MB_Init adds it to the beacon period. */
int MBi_GetBeaconPeriodDispersion(void) {
    u8 mac[6];
    u32 sum;
    int i;

    OS_GetMacAddress(mac);
    for (i = 0, sum = 0; i < 6; i++) {
        sum += mac[i];
    }
    return ((sum + OS_GetVBlankCount()) * 7 % 20);
}

/* MB_Init: carve the WmWork and mbc out of the caller's buffer (each 32-byte
 * aligned), clear them, copy the user name and the parent name, store the
 * user info and set the parent parameters. Refused (2) while
 * isMbInitialized (the SDK's isMbInitialized) is set. Lead: the reconstruction's MB_Init
 * takes (work, user, ggid, tgid, dma), resolves MB_TGID_AUTO and sets the
 * lifetimes and the power-save flag; this one takes the GGID as two halves
 * (ggid0, ggid1) and does none of that. */
int MB_Init(void *work, const MBUserInfo *user, int ggid0, int ggid1, u16 tgid, int dma) {
    volatile unsigned short fill_s;
    volatile int fill_i;
    /* Pointers, not ints: every WmWork store below (parentMaxSize,
     sendBufSize, sendBuf, ...) is add-high / small-offset on the cartridge
     (add r0,r7,#0x1e00 / strh r3,[r0,#0x18]), which mwcc only emits for
     pointer arithmetic. With int bases each offset is materialized from the
     literal pool and stored through a register index, 15 words longer. */
    WmWork *wm;
    MB_CommPWork *mb;
    OSIntrMode irq;
    if ((mbc != 0) && (mbc->isMbInitialized != 0)) {
        return 2;
    }
    wm = (WmWork *)MATH_ROUNDUP((u32)work, 32);
    mb = (MB_CommPWork *)MATH_ROUNDUP((int)wm + sizeof(WmWork), 32);
    irq = OS_DisableInterrupts();
    fill_i = 0;
    data_020a9db0 = dma;
    data_020a9db4 = (char *)wm;
    mbc = &mb->common;
    MI_CpuFill32(wm, fill_i, sizeof(WmWork));
    fill_s = 0;
    MI_CpuFill16(mb, fill_s, sizeof(MB_CommCommonWork));
    /* The user's name and L"multiboot" (MB_Init's static game-name pointer,
     stepped as it is read); 10 halfwords overrun the 8-byte uname into
     gname, which the second copy then overwrites */
    {
        int i = 0;
        u16 *dst = (u16 *)wm->uname;
        u16 c;
        do {
            c = user->name[i];
            if (c == 0) {
                break;
            }
            i++;
            *(dst++) = c;
        } while (i < 0xa);
    }
    {
        u16 *dst = (u16 *)wm->gname;
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
    MI_CpuCopy8(user, &mb->common.user, sizeof(MBUserInfo));
    wm->parentMaxSize = 0x100;
    wm->childMaxSize = 8;
    wm->sendBufSize = 0;
    wm->recvBufSize = 0;
    wm->mpContinuous = 1;
    wm->sendBuf = (char *)mb->common.sendbuf;
    wm->recvBuf = (char *)mb->common.recvbuf;
    {
        WMParentParam *hdr = &wm->parentParam;
        hdr->CS_Flag = 1;
        hdr->KS_Flag = 0;
        hdr->ggid[0] = ggid0;
        hdr->ggid[1] = ggid1;
        hdr->tgid = tgid;
        hdr->entryFlag = 1;
        hdr->multiBootFlag = 0;
        hdr->beaconPeriod = (MBi_GetBeaconPeriodDispersion() + 0xc8);
        hdr->maxEntry = 0xf;
    }
    wm->mpSending = 0;
    wm->initialized = 0;
    mb->common.start_mp_busy = 0;
    OS_RestoreInterrupts(irq);
    mb->common.isMbInitialized = 1;
    return 0;
}

/* MBi_IsCommSizeValid: the size check MB_SetParentCommParam runs. Lead: the
 * reconstruction's checks the parent size (0xe4..0x1fe), the child size
 * (8..0x10) and the buffer they need; this one accepts everything. */
int MBi_IsCommSizeValid(int parentMaxSize, int childMaxSize, int maxEntry) {
    return 1;
}

/* MB_SetParentCommParam: set the MP sizes and the parent's maxEntry. Refused
 * (0) once the wireless layer is initialized, or if MBi_IsCommSizeValid rejects
 * them. The game's link setup func_0203e20c passes (0x1fe, 8, 3). Lead: the
 * reconstruction's takes (sendSize, maxChildren), fixes the child size at 8
 * and disables interrupts around it; this one takes all three sizes and
 * disables nothing. */
int MB_SetParentCommParam(int parentMaxSize, int childMaxSize, int maxEntry) {
    if (WM_WORK->initialized)
        return 0;
    if (MBi_IsCommSizeValid(parentMaxSize, childMaxSize, maxEntry) == 0)
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
int MBi_StartCommon(void) {
    WmWork *base;
    int ret;
    base = WM_WORK;
    base->mpStarted = 0;
    WM_WORK->childBitmap = 0;
    WM_WORK->resetting = 0;
    *(short *)(data_020a9db4 + 0x1e48) = 0; /* in WmWork.pad_1e48: no reader here */
    MBi_SetMaxScanTime(0xa);
    do {
        ret = WM_Initialize(WM_WORK->wmSysBuf, WM_WORK->stateCallback, data_020a9db0);
    } while (ret == 4);
    if (ret != 2)
        return 8;
    WM_SetIndCallback(WM_WORK->stateCallback);
    WM_WORK->initialized = 1;
    return 0;
}

/* MBi_StartParentCore with MB_StartParent: record the channel, clear the
 * parent work from childUser on (keeping the parent callback across the
 * clear), size the blocks and the child request pieces from the MP packet
 * sizes, mark every child as in no file, clear the file table and the
 * pending answers, then hand WM the parent callbacks and buffer sizes and
 * start. Lead: the reconstruction's sets mbc->is_started_ex in a separate
 * MB_StartParent, sizes the request pieces through MBi_SetChildMPMaxSize and
 * reads PXI_IsCallbackReady for useWvrFlag at the end. */
int MB_StartParent(int channel) {
    OSIntrMode saved;
    volatile u16 fill0;
    volatile u16 fill1;
    MBCommPStateCallback cb;
    int i;
    int result;

    saved = OS_DisableInterrupts();

    WM_WORK->parentParam.channel = channel;

    cb = pPwork->parent_callback;

    fill0 = 0;
    MI_CpuFill16(pPwork->childUser, fill0, sizeof(MB_CommPWork) - sizeof(MB_CommCommonWork));

    MB_CommSetParentStateCallback(cb);

    mbc->block_size_max = WM_WORK->parentMaxSize - 6;
    mbc->req_data_piece_size = WM_WORK->childMaxSize - 2;
    mbc->req_data_piece_num = 30 / (WM_WORK->childMaxSize - 2);

    for (i = 0; i < 0xf; i++) {
        pPwork->p_comm_state[i] = 0;
        pPwork->fileid_of_child[i] = -1;
    }
    pPwork->file_num = 0;

    fill1 = 0;
    MI_CpuFill16(pPwork->fileinfo, fill1, sizeof(pPwork->fileinfo));

    MI_CpuClear8(pPwork->req2child, sizeof(pPwork->req2child));

    WM_WORK->mode = 1;
    WM_WORK->userCallback = (void (*)(int, void *))MBi_CommParentCallback;
    WM_WORK->stateCallback = MBi_ParentCallback;

    WM_WORK->parentParam.parentMaxSize = WM_WORK->parentMaxSize;
    WM_WORK->sendBufSize = (WM_WORK->parentParam.parentMaxSize + 0x23) & ~0x1f;

    WM_WORK->parentParam.childMaxSize = WM_WORK->childMaxSize;
    WM_WORK->recvBufSize = (((WM_WORK->parentParam.childMaxSize + 0xe) * 0xf + 0x29) & ~0x1f) << 1;

    MB_InitSendGameInfoStatus();
    result = MBi_StartCommon();

    OS_RestoreInterrupts(saved);

    return result;
}

/* MBi_CommEnd with MBi_CallReset: unless a reset is already under way, stop
 * the scan, mark the reset and call WM_Reset; 0 on OPERATING, 1 if a reset
 * was already running. Lead: the reconstruction's MBi_CommEnd disables
 * interrupts, calls MBi_EndCommon when the manager never started and resets
 * through the task thread when there is one; MBi_CallReset also checks the
 * WM result. */
int MBi_CommEnd(void) {
    WmWork *p = WM_WORK;
    if (p->resetting == 0) {
        int r;
        p->scanContinue = 0;
        /* through the global again: the ROM reloads data_020a9db4 here */
        WM_WORK->resetting = 1;
        r = WM_Reset(WM_WORK->stateCallback);
        if (r == 2)
            return 0;
        return r;
    }
    return 1;
}

/* MB_End: MBi_CommEnd with interrupts disabled. Lead: the reconstruction's
 * also calls OS_Terminate when MB_StartParent was not used. */
void MB_End(void) {
    OSIntrMode saved = OS_DisableInterrupts();
    MBi_CommEnd();
    OS_RestoreInterrupts(saved);
}

/* MBi_SetMPData: send on port 1 (the SDK's WM_PORT_BT) at low priority.
 * tmptt is unused. The later SDK also passes the result to
 * MBi_CheckWmErrcode. */
int MBi_SetMPData(WMCallbackFunc callback, const u16 *sendData, u16 sendDataSize, u16 tmptt,
                  u16 pollbmp) {
    return WM_SetMPDataToPort(callback, sendData, sendDataSize, pollbmp, 1, WM_PRIORITY_LOW);
}

/* MBi_SendMP: queue one MP send. Fails (1) until MP has started or while a
 * reset is under way. A parent (mode 1) sends through stateCallback, a child
 * (mode 2) through MBi_ChildCallback. On WM's OPERATING (2) it sets mpSending and
 * returns 0; any other code is returned as it is. Lead: the reconstruction's
 * has the same two cases, but this parent case first pads the size up to
 * data_0209a088 (6). */
int MBi_SendMP(const void *buf, int len, int pollbmp) {
    WmWork *hdr = WM_WORK;
    u16 size = len;
    u16 pollbitmap = pollbmp;
    int ret;
    int wait;
    if ((hdr->mpStarted == 0) || (hdr->resetting == 1)) {
        return 1;
    }
    switch (hdr->mode) {
    case 1:
        if (size < data_0209a088) {
            size = data_0209a088;
        }
        if (hdr->mpContinuous == 0) {
            wait = 1000;
        } else {
            wait = 0;
        }
        ret = MBi_SetMPData(hdr->stateCallback, (const u16 *)buf, size, wait, pollbitmap);
        /* Two tests of OPERATING, as shipped: one if for both costs a word. */
        if (ret == 2) {
            WM_WORK->mpSending = 1;
        }
        if (ret == 2) {
            ret = 0;
        }
        return ret;

    case 2:
        ret = MBi_SetMPData(MBi_ChildCallback, (const u16 *)buf, size, 0, pollbitmap);
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
 * whole u32; this SDK keeps the GGID as two u16s (WMParentParam) and has a
 * getter for each half, which the beacon senders pass to WM_SetGameInfo. */
u16 func_020688e4(void) {
    return WM_WORK->parentParam.ggid[0];
}

/* parentParam.ggid[1], the other half (see func_020688e4). */
u16 func_020688cc(void) {
    return WM_WORK->parentParam.ggid[1];
}

u16 MBi_GetTgid(void) {
    return WM_WORK->parentParam.tgid;
}

/* The beacon attribute byte for WM_SetGameInfo: entryFlag 1, multiBootFlag
 * 2, KS_Flag 4, CS_Flag 8 (the SDK's WM_ATTR_FLAG_* bits). The beacon senders
 * OR in WM_ATTR_FLAG_MB again. */
u8 MBi_GetAttribute(void) {
    return ((((WM_WORK->parentParam.entryFlag) ? WM_ATTR_FLAG_ENTRY : 0) |
             ((WM_WORK->parentParam.multiBootFlag) ? WM_ATTR_FLAG_MB : 0) |
             ((WM_WORK->parentParam.KS_Flag) ? WM_ATTR_FLAG_KS : 0) |
             ((WM_WORK->parentParam.CS_Flag) ? WM_ATTR_FLAG_CS : 0)));
}

void MBi_SetMaxScanTime(u16 time) {
    SCAN_PARAM->maxChannelTime = time;
}

/* WmWork.parentMaxSize, the parent's MP send size (set by MB_SetParentCommParam).
 * MBi_CommParentCallback uses it to size the request record it clears for a
 * child. Lead: the reconstruction has no getter for p_sendlen. */
int func_0206881c(void) {
    return WM_WORK->parentMaxSize;
}

/* Whether the wireless layer is up: WmWork.initialized == 1, set after
 * WM_Initialize. MB_RegisterFile refuses to register a file before then. */
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
    p.apiid = apiid;
    p.errcode = errcode;
    WM_WORK->userCallback(0xff, &p);
}
