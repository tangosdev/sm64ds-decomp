/* NitroSDK wm_standard.c: the WM state changes. Each call checks the state
 * it starts from, registers callback under its API id and queues the request
 * for ARM7, answering WM_ERRCODE_OPERATING; the callback reports the result.
 * This SDK is older than pret's pokediamond: WM_Enable and WM_Initialize
 * send three words with no misc flags, WM_StartParent sends no powerSave,
 * WM_StartConnect takes no powerSave or authMode, the scan and connect
 * requests are written straight into the ARM9-to-ARM7 command block, and
 * WmCheckParentParameter also checks the MP exchange time. */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/mi/memory.h"
#include "nitro/wm/wm_api.h"
#include "private/wm_views.h"

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NULL 0

#define WM_SIZE_USER_GAMEINFO 0x70
#define WM_SIZE_MP_DATA_MAX 0x200
#define WM_SIZE_KS_PARENT_DATA 0x24
#define WM_SIZE_KS_CHILD_DATA 0x2
#define WM_SIZE_MP_PARENT_PADDING 0x6
#define WM_SIZE_MP_CHILD_PADDING 0x4
#define WM_SIZE_CHILD_SSID 0x18
#define WM_NUM_MAX_CHILD 15

/* The START_SCAN request, written into the command block (fifo9to7). */
typedef struct WMStartScanReq {
    u16 apiid;          /* 0x0 */
    u16 channel;        /* 0x2 */
    WMBssDesc *scanBuf; /* 0x4 */
    u16 maxChannelTime; /* 0x8 */
    u16 bssid[3];       /* 0xa -- u8[6] in the SDK, copied as halfwords */
} WMStartScanReq;

/* The START_CONNECT request, written into the command block. */
typedef struct WMStartConnectReq {
    u16 apiid;                   /* 0x00 */
    u16 reserved;                /* 0x02 */
    WMBssDesc *pInfo;            /* 0x04 */
    u8 ssid[WM_SIZE_CHILD_SSID]; /* 0x08 */
} WMStartConnectReq;

static BOOL WmCheckParentParameter(const WMParentParam *param);

/* READY to STOP: claim the wireless hardware, sending ARM7 its work area,
 * the status block and the ARM7-to-ARM9 FIFO. */
WMErrCode WM_Enable(WMCallbackFunc callback) {
    WMErrCode result;

    result = WM_CheckStateEx(1, WM_STATE_READY);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_ENABLE, callback);
    {
        WMArm9Buf *p = WM_GetSystemWork();

        result =
            WM_SendCommand(WM_APIID_ENABLE, 3, (u32)(p->WM7), (u32)(p->status), (u32)(p->fifo7to9));
        if (result != WM_ERRCODE_SUCCESS)
            return result;
    }
    return WM_ERRCODE_OPERATING;
}

/* STOP back to READY: give the hardware up. */
WMErrCode WM_Disable(WMCallbackFunc callback) {
    WMErrCode result;

    result = WM_CheckStateEx(1, WM_STATE_STOP);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_DISABLE, callback);
    result = WM_SendCommand(WM_APIID_DISABLE, 0);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* STOP to IDLE: power the hardware on. */
WMErrCode WM_PowerOn(WMCallbackFunc callback) {
    WMErrCode result;

    result = WM_CheckStateEx(1, WM_STATE_STOP);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_POWER_ON, callback);
    result = WM_SendCommand(WM_APIID_POWER_ON, 0);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* IDLE back to STOP: power the hardware off. */
WMErrCode WM_PowerOff(WMCallbackFunc callback) {
    WMErrCode result;

    result = WM_CheckStateEx(1, WM_STATE_IDLE);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_POWER_OFF, callback);
    result = WM_SendCommand(WM_APIID_POWER_OFF, 0);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* READY to IDLE in one request: WM_Init carves wmSysBuf, then ARM7 enables
 * and powers on the hardware itself. */
WMErrCode WM_Initialize(void *wmSysBuf, WMCallbackFunc callback, u16 dmaNo) {
    WMErrCode result;

    result = WM_Init(wmSysBuf, dmaNo);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_INITIALIZE, callback);
    {
        WMArm9Buf *p = WM_GetSystemWork();

        result = WM_SendCommand(WM_APIID_INITIALIZE, 3, (u32)(p->WM7), (u32)(p->status),
                                (u32)(p->fifo7to9));
        if (result != WM_ERRCODE_SUCCESS)
            return result;
    }
    return WM_ERRCODE_OPERATING;
}

/* Drop any connection and return to IDLE. */
WMErrCode WM_Reset(WMCallbackFunc callback) {
    WMErrCode result;

    result = WMi_CheckIdle();
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_RESET, callback);
    result = WM_SendCommand(WM_APIID_RESET, 0);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* IDLE back to READY in one request: power off and give the hardware up. */
WMErrCode WM_End(WMCallbackFunc callback) {
    WMErrCode result;

    result = WM_CheckStateEx(1, WM_STATE_IDLE);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_END, callback);
    result = WM_SendCommand(WM_APIID_END, 0);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* In IDLE, hand ARM7 the parent parameters. The MP sizes, with the
 * key-sharing data and padding when KS_Flag is set, must fit
 * WM_SIZE_MP_DATA_MAX; WmCheckParentParameter's verdict is not used. */
WMErrCode WM_SetParentParameter(WMCallbackFunc callback, const WMParentParam *pparaBuf) {
    WMErrCode result;

    result = WM_CheckStateEx(1, WM_STATE_IDLE);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    if (pparaBuf == NULL)
        return WM_ERRCODE_INVALID_PARAM;
    if (pparaBuf->userGameInfoLength > 0) {
        if (pparaBuf->userGameInfo == NULL)
            return WM_ERRCODE_INVALID_PARAM;
    }
    if ((pparaBuf->parentMaxSize +
             (pparaBuf->KS_Flag ? WM_SIZE_KS_PARENT_DATA + WM_SIZE_MP_PARENT_PADDING : 0) >
         WM_SIZE_MP_DATA_MAX) ||
        (pparaBuf->childMaxSize +
             (pparaBuf->KS_Flag ? WM_SIZE_KS_CHILD_DATA + WM_SIZE_MP_CHILD_PADDING : 0) >
         WM_SIZE_MP_DATA_MAX))
        return WM_ERRCODE_INVALID_PARAM;
    WmCheckParentParameter(pparaBuf);
    WM_SetCallbackTable(WM_APIID_SET_P_PARAM, callback);
    DC_StoreRange((void *)pparaBuf, sizeof(WMParentParam));
    if (pparaBuf->userGameInfoLength > 0)
        DC_StoreRange(pparaBuf->userGameInfo, pparaBuf->userGameInfoLength);
    result = WM_SendCommand(WM_APIID_SET_P_PARAM, 1, (u32)pparaBuf);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* The user game info length, beacon period (10..1000) and channel (1..14),
 * and an MP exchange that fits 5600 microseconds: the parent's send plus
 * maxEntry children's replies, at 4 microseconds a byte with each frame's
 * overhead. */
static BOOL WmCheckParentParameter(const WMParentParam *param) {
    if (param->userGameInfoLength > WM_SIZE_USER_GAMEINFO)
        return FALSE;
    if ((param->beaconPeriod < 10) || (param->beaconPeriod > 1000))
        return FALSE;
    if ((param->channel < 1) || (param->channel > 14))
        return FALSE;
    if ((0x14a + (param->parentMaxSize + 0x26) * 4) +
            param->maxEntry * (0x70 + (param->childMaxSize + 0x20) * 4) >
        5600)
        return FALSE;
    return TRUE;
}

/* IDLE to PARENT, with the parameters WM_SetParentParameter set. */
WMErrCode WM_StartParent(WMCallbackFunc callback) {
    WMErrCode result;

    result = WM_CheckStateEx(1, WM_STATE_IDLE);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_START_PARENT, callback);
    result = WM_SendCommand(WM_APIID_START_PARENT, 0);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* From IDLE, CLASS1 or SCAN, scan one channel (1..14) for parents into
 * param->scanBuf. */
WMErrCode WM_StartScan(WMCallbackFunc callback, const WMScanParam *param) {
    WMErrCode result;

    result = WM_CheckStateEx(3, WM_STATE_IDLE, WM_STATE_CLASS1, WM_STATE_SCAN);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    if (param == NULL)
        return WM_ERRCODE_INVALID_PARAM;
    if (param->scanBuf == NULL)
        return WM_ERRCODE_INVALID_PARAM;
    if ((param->channel < 1) || (param->channel > 14))
        return WM_ERRCODE_INVALID_PARAM;
    DC_StoreRange((void *)param, sizeof(WMScanParam));
    WM_SetCallbackTable(WM_APIID_START_SCAN, callback);
    {
        WMStartScanReq *req = (WMStartScanReq *)WM_GetSystemWork()->fifo9to7;

        req->apiid = WM_APIID_START_SCAN;
        req->channel = param->channel;
        req->scanBuf = param->scanBuf;
        req->maxChannelTime = param->maxChannelTime;
        req->bssid[0] = param->bssid[0];
        req->bssid[1] = param->bssid[1];
        req->bssid[2] = param->bssid[2];
        result = WM_SendCommand(WM_APIID_START_SCAN, 0);
        if (result != WM_ERRCODE_SUCCESS)
            return result;
    }
    return WM_ERRCODE_OPERATING;
}

/* From IDLE, connect as a child to the parent pInfo describes, with the
 * child ssid (zeroed when there is none). */
WMErrCode WM_StartConnect(WMCallbackFunc callback, const WMBssDesc *pInfo, const u8 *ssid) {
    WMErrCode result;

    result = WM_CheckStateEx(1, WM_STATE_IDLE);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    if (pInfo == NULL)
        return WM_ERRCODE_INVALID_PARAM;
    DC_StoreRange((void *)pInfo, sizeof(WMBssDesc));
    WM_SetCallbackTable(WM_APIID_START_CONNECT, callback);
    {
        WMStartConnectReq *req = (WMStartConnectReq *)WM_GetSystemWork()->fifo9to7;

        req->apiid = WM_APIID_START_CONNECT;
        req->pInfo = (WMBssDesc *)pInfo;
        if (ssid != NULL) {
            MI_CpuCopy8(ssid, req->ssid, WM_SIZE_CHILD_SSID);
        } else {
            MI_CpuClear8(req->ssid, WM_SIZE_CHILD_SSID);
        }
        result = WM_SendCommand(WM_APIID_START_CONNECT, 0);
        if (result != WM_ERRCODE_SUCCESS)
            return result;
    }
    return WM_ERRCODE_OPERATING;
}

/* Cut a connection: a parent cuts the child aid (1..15, connected), a
 * child (aid 0) its parent. */
WMErrCode WM_Disconnect(WMCallbackFunc callback, u16 aid) {
    WMErrCode result;
    WMArm9Buf *p = WM_GetSystemWork();

    result = WM_CheckStateEx(5, WM_STATE_PARENT, WM_STATE_MP_PARENT, WM_STATE_CHILD,
                             WM_STATE_MP_CHILD, WM_STATE_DCF_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    if ((p->status->state == WM_STATE_PARENT) || (p->status->state == WM_STATE_MP_PARENT)) {
        if ((aid < 1) || (aid > WM_NUM_MAX_CHILD))
            return WM_ERRCODE_INVALID_PARAM;
        DC_InvalidateRange(&(p->status->child_bitmap), 2);
        if (!(p->status->child_bitmap & (0x0001 << aid)))
            return WM_ERRCODE_NO_CHILD;
    } else {
        if (aid != 0)
            return WM_ERRCODE_INVALID_PARAM;
    }
    WM_SetCallbackTable(WM_APIID_DISCONNECT, callback);
    result = WM_SendCommand(WM_APIID_DISCONNECT, 1, (u32)(0x0001 << aid));
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}
