#include "nitro/wm/wm_api.h"

extern void func_02040504(unsigned short *cb);
extern int data_020a0f94;

/* Disable the wireless hardware: WM_Disable sends WM command 4 (the
   SDK's WM_APIID_DISABLE, stop state only) with func_02040504 as the
   callback. As func_0203fc74 otherwise. */
int func_0203fcec(void) {
    if (WM_Disable((WMCallbackFunc)func_02040504) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
