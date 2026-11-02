//cpp
#include "types.h"
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

extern int *WM_GetSystemWork(void);
extern int WM_CheckStateEx(int a, int b, int c);

/* NitroSDK WM_GetMPSendBufferSize(): 0 unless the system is a parent or a
 * child (states 7, 8) that has not started MP yet; otherwise mp_maxSendSize
 * rounded up to 32 bytes. WMArm9Buf / WMStatus are in
 * include/private/wm_views.h. */
u32 WM_GetMPSendBufferSize(void) {
    WMArm9Buf *r4;
    WMStatus *p;

    r4 = (WMArm9Buf *)WM_GetSystemWork();
    if (WM_CheckStateEx(2, 7, 8) != 0)
        return 0;

    CP15::InvalidateDataCache((u32)&r4->status->mp_flag, 4);

    p = r4->status;
    if (p->mp_flag == 1)
        return 0;

    CP15::InvalidateDataCache((u32)&p->mp_maxSendSize, 4);

    return (r4->status->mp_maxSendSize + 0x1f) & ~0x1f;
}
}
