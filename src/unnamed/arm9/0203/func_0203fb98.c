#include "nitro/wm/wm_api.h"

extern void func_02040014(char *cb);

/* Start as parent: WM_StartParent sends WM command 8 (the SDK's
   WM_APIID_START_PARENT) with func_02040014 as the callback. 1 when the
   request is accepted (2). */
int func_0203fb98(void) {
    return WM_StartParent((WMCallbackFunc)func_02040014) == WM_ERRCODE_OPERATING;
}
