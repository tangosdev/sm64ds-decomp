//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "private/wm_views.h"

extern "C" {

/* NitroSDK wm_standard.c WM_Disconnect(callback, aid), by body against pret's
 * pokediamond WM_standard.s: in a connected state, a parent (7, 9) cuts child
 * aid (1..15 and connected, else 6 or 7) and a child (aid 0) its parent. It
 * sends API 0xd (DISCONNECT) with 1 << aid. */
int WM_Disconnect(void *callback, unsigned int idx) {
    WMArm9Buf *sys;
    WMStatus *status;
    int r;
    sys = WM_GetSystemWork();
    r = WM_CheckStateEx(5, 7, 9, 8, 0xa, 0xb);
    if (r != 0) return r;
    status = sys->status;
    {
        u16 v = status->state;
        if (v == 7 || v == 9) {
            if (idx < 1 || idx > 0xf)
                return 6;
            DC_InvalidateRange(&status->child_bitmap, 2);
            if ((sys->status->child_bitmap & (1u << idx)) == 0)
                return 7;
        } else {
            if (idx != 0) return 6;
        }
    }
    WM_SetCallbackTable(0xd, callback);
    r = WM_SendCommand(0xd, 1, 1u << idx);
    if (r == 0) return 2;
    return r;
}
}
