//cpp
#include "types.h"
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

/* NitroSDK wm_mp.c WM_StartMPEx(callback, recvBuf, recvBufSize, sendBuf,
 * sendBufSize, mpFreq, defaultRetryCount, minPollBmpMode, singlePacketMode,
 * fixFreqMode, ignoreFatalError): pret's pokediamond WM_mp.s has the same
 * eleven arguments, and its WMi_StartMP makes the same checks. In this older
 * SDK one function does both: in PARENT or CHILD (7, 8) with MP not started
 * (else 3), it checks recvBufSize against WM_GetMPReceiveBufferSize and
 * 64-byte alignment and sendBufSize against WM_GetMPSendBufferSize and
 * 32-byte alignment (6). Then it sends API 0xe (START_MP) with recvBufSize
 * in halfwords. */
extern int WM_GetMPReceiveBufferSize(void);
extern int WM_GetMPSendBufferSize(void);
extern void WM_SetCallbackTable(int i, int val);

int WM_StartMPEx(int arg0, int arg1, int arg2, int arg3, u16 arg4, u16 arg5, u16 arg6, int arg7,
                 int arg8, int arg9, int arg10) {
    WMArm9Buf *base;
    int r;

    base = WM_GetSystemWork();
    r = WM_CheckStateEx(2, 7, 8);
    if (r != 0)
        return r;
    CP15::InvalidateDataCache((unsigned int)&base->status->mp_flag, 4);
    if (base->status->mp_flag == 1)
        return 3;
    if (arg2 < WM_GetMPReceiveBufferSize())
        return 6;
    if (arg2 & 0x3f)
        return 6;
    if (arg4 < WM_GetMPSendBufferSize())
        return 6;
    if (arg4 & 0x1f)
        return 6;
    WM_SetCallbackTable(0xe, arg0);
    /* sendBufSize goes out as a u32 word, the way the SDK casts each request
       word; passed bare, the variadic promotion reuses its register and the
       argument stores come out in another order. */
    r = WM_SendCommand(0xe, 0xa, arg1, (unsigned int)arg2 >> 1, arg3, (u32)arg4, arg5, arg6, arg7,
                       arg8, arg9, arg10);
    if (r == 0)
        r = 2;
    return r;
}
}
