//cpp
/* NitroSDK wm_mp.c WM_SetMPDataToPortEx(callback, arg, sendData,
 * sendDataSize, destBitmap, port, prio), by body against pret's pokediamond
 * WM_mp.s: in MP_PARENT or MP_CHILD, queue sendData for the AIDs in
 * destBitmap on a port. It refuses a missing buffer (6), a parent with no
 * children (7), and a size of 0 or one past mp_maxSendSize less the header
 * (6), then flushes the data and sends API 0xf (SET_MP_DATA) with pret's
 * argument order. */
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

extern int *WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, ...);
extern int WM_SendCommand(int a, int b, int c, int d, int e, int f, int g, int h, int i);

/* WM_GetSystemWork's WMArm9Buf and its WMStatus (include/private/wm_views.h), the fields read here.
 * child_bitmap follows WM_GetLinkLevel and include/WmWork.h; mp_maxSendSize
 * is the SDK field just before WM_GetMPReceiveBufferSize's mp_maxRecvSize (0x190), and
 * WM_GetMPSendBufferSize rounds it up for the send-buffer size. 0x86 has no name in
 * any view yet, so it is only invalidated by offset below. */
int WM_SetMPDataToPortEx(int p0, int p1, int p2, int p3, unsigned short p4, unsigned short p5,
                         unsigned short p6) {
    int maxSendSize;
    int x;
    WMArm9Buf *handle;
    int ret;
    int aid;
    int flag;

    x = 1;
    handle = (WMArm9Buf *)WM_GetSystemWork();
    ret = WM_CheckStateEx(2, 9, 0xa);
    if (ret != 0)
        return ret;

    CP15::InvalidateDataCache((unsigned int)&handle->status->mp_maxSendSize, 2);
    maxSendSize = handle->status->mp_maxSendSize;
    CP15::InvalidateDataCache((unsigned int)&handle->status->aid, 2);

    aid = handle->status->aid;
    flag = (aid == 0) ? x : 0;
    if (flag == 1) {
        CP15::InvalidateDataCache((unsigned int)&handle->status->child_bitmap, 2);
        x = handle->status->child_bitmap;
        CP15::InvalidateDataCache((unsigned int)&handle->status->mp_readyBitmap, 2);
    }

    if (p2 == 0)
        return 6;
    if (x == 0)
        return 7;
    if (p3 + (flag != 0 ? 4 : 2) > maxSendSize)
        return 6;
    if (p3 == 0)
        return 6;

    CP15::FlushDataCache(p2, p3);
    ret = WM_SendCommand(0xf, 7, p2, p3, p4, p5, p6, p0, p1);
    if (ret == 0)
        return 2;
    return ret;
}
}
