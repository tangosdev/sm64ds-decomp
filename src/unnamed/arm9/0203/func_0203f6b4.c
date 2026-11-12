#include "nitro/os/system.h"
#include "nitro/wm/wm_api.h"
extern void func_0203f920(unsigned short *cb);
/* Shut the wireless hardware down after a measurement: WM_End
   sends WM_APIID_END (2) through callback slot 2, with
   func_0203f920 as the callback, and anything but 2 (the SDK's
   WM_ERRCODE_OPERATING, "accepted") halts. */
void func_0203f6b4(void) {
    if (WM_End((WMCallbackFunc)func_0203f920) == WM_ERRCODE_OPERATING)
        return;
    OS_Terminate();
}
