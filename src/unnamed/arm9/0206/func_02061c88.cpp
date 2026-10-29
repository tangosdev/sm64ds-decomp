//cpp
#include "IRQ.h"

extern "C" {

extern int func_0206152c(void);
extern char *WM_GetSystemWork(void);

/* WM_GetSystemWork's WMArm9Buf, the per-port tables this function fills,
 * with NitroSDK's names (func_02062d10, the data-sharing send callback,
 * looks its argument up in the second one by port). */
typedef struct WMArm9Buf {
    char pad_00[0xac];
    void *portCallbackTable[16];    /* 0x0ac */
    void *portCallbackArgument[16]; /* 0x0ec */
} WMArm9Buf;

/* NitroSDK WM_SetPortCallback(port, callback, arg): unless WM is not
 * initialised (func_0206152c's error), store the port's receive callback
 * and its argument with interrupts off. */
int func_02061c88(int port, int callback, int arg) {
    int token = IRQ::Disable();
    int r = func_0206152c();
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
