/* NitroSDK wm_etc.c: the WM requests that only pass their arguments to
 * ARM7. Each one checks the state it starts from, registers callback under
 * its API id and queues the request, answering WM_ERRCODE_OPERATING; the
 * callback reports the result. This SDK is older than pret's pokediamond:
 * WM_SetEntry, WM_SetWEPKey and WM_SetWEPKeyEx are absent, WM_SetGameInfo
 * takes the GGID as its two halfwords and copies the game info a word at a
 * time, WM_SetLifeTime has no mpLifeTime, and WM_MeasureChannel writes its
 * request straight into the ARM9-to-ARM7 command block. */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/mi/memory.h"
#include "nitro/wm/wm_api.h"
#include "private/wm_views.h"

#define NULL 0

#define WM_SIZE_USER_GAMEINFO 0x70
#define WM_SIZE_TEMP_USR_GAME_INFO_BUF 128

/* The MEASURE_CHANNEL request, written into the command block (fifo9to7). */
typedef struct WMMeasureChannelReq {
    u16 apiid;       /* 0x0 */
    u16 ccaMode;     /* 0x2 */
    u16 edThreshold; /* 0x4 */
    u16 channel;     /* 0x6 */
    u16 measureTime; /* 0x8 */
} WMMeasureChannelReq;

/* The user game info, copied here for ARM7 to read with the beacon. */
static u32 tmpUserGameInfoBuf[WM_SIZE_TEMP_USR_GAME_INFO_BUF / sizeof(u32)];

/* PARENT or MP_PARENT: change the game info the beacon carries. */
WMErrCode WM_SetGameInfo(WMCallbackFunc callback, const u16 *userGameInfo, u16 userGameInfoSize,
                         u16 ggid0, u16 ggid1, u16 tgid, u8 attr) {
    WMErrCode result;

    result = WM_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_MP_PARENT);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    if (userGameInfo == NULL)
        return WM_ERRCODE_INVALID_PARAM;
    if (userGameInfoSize > WM_SIZE_USER_GAMEINFO)
        return WM_ERRCODE_INVALID_PARAM;
    MI_CpuCopy32(userGameInfo, tmpUserGameInfoBuf, userGameInfoSize);
    DC_StoreRange((void *)tmpUserGameInfoBuf, userGameInfoSize);
    WM_SetCallbackTable(WM_APIID_SET_GAMEINFO, callback);
    result = WM_SendCommand(WM_APIID_SET_GAMEINFO, 6, (u32)tmpUserGameInfoBuf,
                            (u32)userGameInfoSize, (u32)ggid0, (u32)ggid1, (u32)tgid, (u32)attr);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* Whether ARM7 reports each beacon it sends (flag 1) or not (0). */
WMErrCode WM_SetBeaconIndication(WMCallbackFunc callback, u16 flag) {
    WMErrCode result;

    result = WMi_CheckIdle();
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    if ((0 != flag) && (1 != flag))
        return WM_ERRCODE_INVALID_PARAM;
    WM_SetCallbackTable(WM_APIID_SET_BEACON_IND, callback);
    result = WM_SendCommand(WM_APIID_SET_BEACON_IND, 1, (u32)flag);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* How long a silent peer stays connected, in each of the three tables. */
WMErrCode WM_SetLifeTime(WMCallbackFunc callback, u16 tableNumber, u16 camLifeTime,
                         u16 frameLifeTime) {
    WMErrCode result;

    result = WMi_CheckIdle();
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_SET_LIFETIME, callback);
    result = WM_SendCommand(WM_APIID_SET_LIFETIME, 3, (u32)tableNumber, (u32)camLifeTime,
                            (u32)frameLifeTime);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* IDLE: measure how busy channel is for measureTime milliseconds. */
WMErrCode WM_MeasureChannel(WMCallbackFunc callback, u16 ccaMode, u16 edThreshold, u16 channel,
                            u16 measureTime) {
    WMErrCode result;
    WMArm9Buf *p = WM_GetSystemWork();

    result = WM_CheckStateEx(1, WM_STATE_IDLE);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(WM_APIID_MEASURE_CHANNEL, callback);
    {
        WMMeasureChannelReq *req = (WMMeasureChannelReq *)p->fifo9to7;

        req->apiid = WM_APIID_MEASURE_CHANNEL;
        req->ccaMode = ccaMode;
        req->edThreshold = edThreshold;
        req->channel = channel;
        req->measureTime = measureTime;
        result = WM_SendCommand(WM_APIID_MEASURE_CHANNEL, 0);
        if (result != WM_ERRCODE_SUCCESS)
            return result;
    }
    return WM_ERRCODE_OPERATING;
}
