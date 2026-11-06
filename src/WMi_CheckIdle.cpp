//cpp
/* NitroSDK wm_system.c's two precondition checks. data_020a89a8 is the
 * flag WM_Init sets once; data_020a89ac the ARM9 system work buffer,
 * whose second word points at the status block ARM7 shares (its first
 * halfword the WM state). */
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

extern unsigned short data_020a89a8;
extern WMArm9Buf *data_020a89ac;

/* WMi_CheckInitialized: WM_ERRCODE_ILLEGAL_STATE (3) before WM_Init, else
 * WM_ERRCODE_SUCCESS. */
int WMi_CheckInitialized(void) {
    return data_020a89a8 ? 0 : 3;
}

/* WMi_CheckIdle: also ILLEGAL_STATE while the state, read fresh from the
 * status block, is still READY (0) or STOP (1). */
int WMi_CheckIdle(void) {
    int result = WMi_CheckInitialized();
    if (result != 0)
        return result;
    CP15::InvalidateDataCache((unsigned int)data_020a89ac->status, 2);
    return data_020a89ac->status->state <= 1 ? 3 : 0;
}
}
