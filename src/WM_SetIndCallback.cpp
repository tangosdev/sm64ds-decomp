//cpp
/* NitroSDK wm_sync.c WM_SetIndCallback: install callback for the
 * indications ARM7 sends outside any request's own callback, with
 * interrupts off. WM_ERRCODE_ILLEGAL_STATE (3) before WM_Init, else
 * WM_ERRCODE_SUCCESS. */
#include "nitro/os/system.h"
#include "private/wm_views.h"

extern "C" {

extern int WMi_CheckInitialized(void);

int WM_SetIndCallback(void *callback) {
    OSIntrMode enabled = OS_DisableInterrupts();
    int result = WMi_CheckInitialized();
    if (result != 0) {
        OS_RestoreInterrupts(enabled);
        return result;
    }
    WM_GetSystemWork()->indCallback = callback;
    OS_RestoreInterrupts(enabled);
    return 0;
}
}
