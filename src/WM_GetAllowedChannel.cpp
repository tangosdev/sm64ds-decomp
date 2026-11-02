//cpp
/* NitroSDK wm_sync.c WM_GetAllowedChannel: the bitmap of channels the
 * console may use (bit n-1 for channel n), read from ARM7's status block
 * after invalidating it from the data cache; 0 unless WM is initialized
 * and past READY and STOP (WMi_CheckIdle). */
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

extern void *WM_GetSystemWork(void);
extern int WMi_CheckIdle(void);

unsigned short WM_GetAllowedChannel(void) {
    WMArm9Buf *sys = (WMArm9Buf *)WM_GetSystemWork();
    if (WMi_CheckIdle() != 0)
        return 0;
    CP15::InvalidateDataCache((unsigned)&sys->status->allowedChannel, 2);
    return sys->status->allowedChannel;
}
}
