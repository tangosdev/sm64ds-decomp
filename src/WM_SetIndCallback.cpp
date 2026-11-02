//cpp
/* NitroSDK wm_sync.c WM_SetIndCallback: install callback for the
 * indications ARM7 sends outside any request's own callback, with
 * interrupts off. WM_ERRCODE_ILLEGAL_STATE (3) before WM_Init, else
 * WM_ERRCODE_SUCCESS. */
#include "IRQ.h"
#include "private/wm_views.h"

extern "C" {

extern int WMi_CheckInitialized(void);
extern void *WM_GetSystemWork(void);

int WM_SetIndCallback(int callback) {
    int enabled = IRQ::Disable();
    int result = WMi_CheckInitialized();
    if (result != 0) {
        IRQ::Restore(enabled);
        return result;
    }
    ((WMArm9Buf *)WM_GetSystemWork())->indCallback = (void *)callback;
    IRQ::Restore(enabled);
    return 0;
}
}
