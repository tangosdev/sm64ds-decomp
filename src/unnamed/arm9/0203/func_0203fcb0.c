#include "nitro/wm/wm_api.h"

extern void func_02040408(unsigned short *cb);
extern int data_020a0f94;

/* Power the wireless hardware on: WM_PowerOn sends WM command 5 (the
   SDK's WM_APIID_POWER_ON, stop state only) with func_02040408 as the
   callback. As func_0203fc74 otherwise. */
int func_0203fcb0(void) {
    if (WM_PowerOn((WMCallbackFunc)func_02040408) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
