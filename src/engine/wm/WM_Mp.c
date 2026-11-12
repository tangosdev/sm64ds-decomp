/* NitroSDK wm_mp.c: start MP communication and queue MP data. This SDK is
 * older than pret's pokediamond: WM_StartMPEx makes WMi_StartMP's checks
 * itself and sends its eleven arguments as request words, and
 * WM_SetMPDataToPortEx limits the size by the status block's mp_maxSendSize. */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/wm/wm_api.h"
#include "private/wm_views.h"

typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define NULL 0

/* In PARENT or CHILD with MP not started, check recvBufSize against
 * WM_GetMPReceiveBufferSize and 64-byte alignment and sendBufSize against
 * WM_GetMPSendBufferSize and 32-byte alignment, then send START_MP with
 * recvBufSize in halfwords. */
WMErrCode WM_StartMPEx(WMCallbackFunc callback, u16 *recvBuf, u16 recvBufSize, u16 *sendBuf,
                       u16 sendBufSize, u16 mpFreq, u16 defaultRetryCount, BOOL minPollBmpMode,
                       BOOL singlePacketMode, BOOL fixFreqMode, BOOL ignoreFatalError) {
    WMArm9Buf *p;
    WMErrCode result;

    p = WM_GetSystemWork();
    result = WM_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    DC_InvalidateRange(&p->status->mp_flag, 4);
    if (p->status->mp_flag == TRUE)
        return WM_ERRCODE_ILLEGAL_STATE;
    if (recvBufSize < WM_GetMPReceiveBufferSize())
        return WM_ERRCODE_INVALID_PARAM;
    if (recvBufSize & 0x3f)
        return WM_ERRCODE_INVALID_PARAM;
    if (sendBufSize < WM_GetMPSendBufferSize())
        return WM_ERRCODE_INVALID_PARAM;
    if (sendBufSize & 0x1f)
        return WM_ERRCODE_INVALID_PARAM;
    WM_SetCallbackTable(WM_APIID_START_MP, callback);
    result =
        WM_SendCommand(WM_APIID_START_MP, 10, (u32)recvBuf, (u32)recvBufSize / 2, (u32)sendBuf,
                       (u32)sendBufSize, (u32)mpFreq, (u32)defaultRetryCount, (u32)minPollBmpMode,
                       (u32)singlePacketMode, (u32)fixFreqMode, (u32)ignoreFatalError);
    if (result == WM_ERRCODE_SUCCESS)
        result = WM_ERRCODE_OPERATING;
    return result;
}

/* MP parent only: send API 0x23 with one word. Later SDKs call 0x23
 * SET_MP_PARAMETER and send a whole parameter block; this SDK's id list is
 * not known that far. */
WMErrCode func_0206259c(WMCallbackFunc callback, u32 param) {
    WMErrCode result;

    (void)WM_GetSystemWork();
    result = WM_CheckStateEx(1, WM_STATE_MP_PARENT);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    WM_SetCallbackTable(0x23, callback);
    result = WM_SendCommand(0x23, 1, param);
    if (result == WM_ERRCODE_SUCCESS)
        result = WM_ERRCODE_OPERATING;
    return result;
}

/* In MP_PARENT or MP_CHILD, queue sendData for the AIDs in destBitmap on a
 * port. It refuses a missing buffer, a parent with no children, and a size of
 * 0 or past mp_maxSendSize less the MP header (4 bytes from a parent, 2 from
 * a child), then flushes the data and sends SET_MP_DATA. */
WMErrCode WM_SetMPDataToPortEx(WMCallbackFunc callback, void *arg, const u16 *sendData,
                               u16 sendDataSize, u16 destBitmap, u16 port, u16 prio) {
    int maxSendSize;
    u16 childBitmap;
    WMArm9Buf *p;
    WMErrCode result;
    BOOL isParent;

    childBitmap = 0x0001;
    p = WM_GetSystemWork();
    result = WM_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
        return result;

    DC_InvalidateRange(&p->status->mp_maxSendSize, 2);
    maxSendSize = p->status->mp_maxSendSize;
    DC_InvalidateRange(&p->status->aid, 2);
    isParent = (p->status->aid == 0) ? TRUE : FALSE;
    if (isParent == TRUE) {
        DC_InvalidateRange(&p->status->child_bitmap, 2);
        childBitmap = p->status->child_bitmap;
        DC_InvalidateRange(&p->status->mp_readyBitmap, 2);
    }

    if (sendData == NULL)
        return WM_ERRCODE_INVALID_PARAM;
    if (childBitmap == 0)
        return WM_ERRCODE_NO_CHILD;
    if (sendDataSize + (isParent ? 4 : 2) > maxSendSize)
        return WM_ERRCODE_INVALID_PARAM;
    if (sendDataSize == 0)
        return WM_ERRCODE_INVALID_PARAM;

    DC_StoreRange((void *)sendData, sendDataSize);
    result = WM_SendCommand(WM_APIID_SET_MP_DATA, 7, (u32)sendData, (u32)sendDataSize,
                            (u32)destBitmap, (u32)port, (u32)prio, (u32)callback, (u32)arg);
    if (result != WM_ERRCODE_SUCCESS)
        return result;
    return WM_ERRCODE_OPERATING;
}

/* WM_SetMPDataToPortEx with no callback argument. */
WMErrCode WM_SetMPDataToPort(WMCallbackFunc callback, const u16 *sendData, u16 sendDataSize,
                             u16 destBitmap, u16 port, u16 prio) {
    return WM_SetMPDataToPortEx(callback, NULL, sendData, sendDataSize, destBitmap, port, prio);
}
