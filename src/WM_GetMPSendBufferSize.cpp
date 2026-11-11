//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "private/wm_views.h"

extern "C" {

/* NitroSDK WM_GetMPSendBufferSize(): 0 unless the system is a parent or a
 * child (states 7, 8) that has not started MP yet; otherwise mp_maxSendSize
 * rounded up to 32 bytes. WMArm9Buf / WMStatus are in
 * include/private/wm_views.h. */
int WM_GetMPSendBufferSize(void) {
    WMArm9Buf *r4;
    WMStatus *p;

    r4 = WM_GetSystemWork();
    if (WM_CheckStateEx(2, 7, 8) != 0)
        return 0;

    DC_InvalidateRange(&r4->status->mp_flag, 4);

    p = r4->status;
    if (p->mp_flag == 1)
        return 0;

    DC_InvalidateRange(&p->mp_maxSendSize, 4);

    return (r4->status->mp_maxSendSize + 0x1f) & ~0x1f;
}
}
