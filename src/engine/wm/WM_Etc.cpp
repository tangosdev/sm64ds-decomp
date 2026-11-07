//cpp
/* NitroSDK wm_etc.c: the WM requests that only pass their arguments to
 * ARM7. Each one registers callback for its API id and sends the request;
 * WM_ERRCODE_OPERATING (2) once it is queued. This SDK is older than pret's
 * pokediamond: WM_SetEntry, WM_SetWEPKey and WM_SetWEPKeyEx are absent,
 * WM_SetLifeTime sends three parameters, not four, and WM_SetGameInfo six,
 * not five. */
#include "types.h"
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

extern int data_020a9440[]; /* the game-info buffer ARM7 reads (pret's UNK_021D68C0) */
extern int WMi_CheckIdle(void);
extern void MultiCopy_Int(int *src, int *dst, int len);

/* NitroSDK WM_SetGameInfo: refuse unless WM is PARENT (7) or MP_PARENT (9),
 * copy the user game info (at most 0x70 bytes) to the buffer ARM7 reads,
 * flush it and send API 0x18 (SET_GAMEINFO) with the buffer, its size and
 * the remaining arguments. */
int WM_SetGameInfo(int callback, int *userGameInfo, int userGameInfoSize, int ggid, u16 arg5,
                   u16 arg6, u8 arg7) {
    int ret = WM_CheckStateEx(2, 7, 9);
    if (ret != 0)
        return ret;
    if (userGameInfo == 0)
        return 6;
    if ((unsigned int)userGameInfoSize > 0x70)
        return 6;
    MultiCopy_Int(userGameInfo, data_020a9440, userGameInfoSize);
    CP15::FlushDataCache((unsigned int)data_020a9440, userGameInfoSize);
    WM_SetCallbackTable(0x18, (void *)callback);
    ret = WM_SendCommand(0x18, 6, data_020a9440, userGameInfoSize, ggid, arg5, arg6, arg7);
    if (ret == 0)
        ret = 2;
    return ret;
}

/* NitroSDK WM_SetBeaconIndication: with WM idle, send API 0x19
 * (SET_BEACON_IND) with flag, which must be 0 or 1. */
int WM_SetBeaconIndication(void *callback, int flag) {
    int r = WMi_CheckIdle();
    if (r != 0)
        return r;
    if (flag != 0 && flag != 1)
        return 6;
    WM_SetCallbackTable(0x19, callback);
    r = WM_SendCommand(0x19, 1, flag);
    if (r == 0)
        return 2;
    return r;
}

/* NitroSDK WM_SetLifeTime: with WM idle, send API 0x1d (SET_LIFETIME) with
 * the three lifetimes. */
int WM_SetLifeTime(void *callback, int b, int c, int d) {
    int r = WMi_CheckIdle();
    if (r != 0)
        return r;
    WM_SetCallbackTable(0x1d, callback);
    r = WM_SendCommand(0x1d, 3, b, c, d);
    if (r == 0)
        return 2;
    return r;
}

/* NitroSDK WM_MeasureChannel: measure how busy channel is. Refuse unless WM
 * is IDLE (2), register callback for API 0x1e (MEASURE_CHANNEL), write the
 * request into the ARM9-to-ARM7 FIFO buffer (the API id, then ccaMode,
 * edThreshold, channel and measureTime in milliseconds) and send it. */
int WM_MeasureChannel(void *callback, int ccaMode, int edThreshold, int channel,
                      unsigned short measureTime) {
    WMArm9Buf *sys;
    short *request;
    int result;

    sys = WM_GetSystemWork();
    result = WM_CheckStateEx(1, 2);
    if (result != 0)
        return result;
    WM_SetCallbackTable(0x1e, callback);
    request = (short *)sys->fifo9to7;
    request[0] = 0x1e;
    request[1] = ccaMode;
    request[2] = edThreshold;
    request[3] = channel;
    request[4] = measureTime;
    result = WM_SendCommand(0x1e, 0);
    if (result != 0)
        return result;
    return 2;
}
}
