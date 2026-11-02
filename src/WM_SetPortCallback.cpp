//cpp
#include "IRQ.h"
#include "private/wm_views.h"

extern "C" {

extern int WMi_CheckInitialized(void);
extern char *WM_GetSystemWork(void);

/* NitroSDK wm_sync.c WM_SetPortCallback: store port's receive callback
 * and its argument, with interrupts off. ILLEGAL_STATE (3) before WM_Init
 * (WMi_CheckInitialized), else WM_ERRCODE_SUCCESS. */
int WM_SetPortCallback(int port, int callback, int arg) {
    int token = IRQ::Disable();
    int r = WMi_CheckInitialized();
    if (r != 0) {
        IRQ::Restore(token);
        return r;
    }
    {
        WMArm9Buf *sys = (WMArm9Buf *)WM_GetSystemWork();
        sys->portCallbackTable[port] = (void *)callback;
        sys->portCallbackArgument[port] = (void *)arg;
    }
    IRQ::Restore(token);
    return 0;
}
}
