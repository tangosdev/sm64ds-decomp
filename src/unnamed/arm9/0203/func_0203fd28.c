#include "nitro/wm/wm_api.h"

extern void func_020405b4(unsigned short *cb);
extern int data_020a0f94;

/* Enable the wireless hardware: WM_Enable sends WM command 3 (the
   SDK's WM_APIID_ENABLE, ready state only) with func_020405b4 as the
   callback. As func_0203fc74 otherwise. */
int func_0203fd28(void) {
    if (WM_Enable((WMCallbackFunc)func_020405b4) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
