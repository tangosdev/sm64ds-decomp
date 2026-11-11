//cpp
#include "nitro/os/system.h"
#include "private/wm_views.h"

extern "C" {

extern int WMi_CheckInitialized(void);

/* NitroSDK wm_sync.c WM_SetPortCallback: store port's receive callback
 * and its argument, with interrupts off. ILLEGAL_STATE (3) before WM_Init
 * (WMi_CheckInitialized), else WM_ERRCODE_SUCCESS. */
int WM_SetPortCallback(int port, void *callback, void *arg) {
    OSIntrMode token = OS_DisableInterrupts();
    int r = WMi_CheckInitialized();
    if (r != 0) {
        OS_RestoreInterrupts(token);
        return r;
    }
    {
        WMArm9Buf *sys = WM_GetSystemWork();
        sys->portCallbackTable[port] = callback;
        sys->portCallbackArgument[port] = arg;
    }
    OS_RestoreInterrupts(token);
    return 0;
}
}
