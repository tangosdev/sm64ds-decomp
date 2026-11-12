#include "nitro/wm/wm_api.h"

extern void func_02040384(void *arg);
extern int data_020a0f94;

/* Install func_02040384 as the WM indication callback. A refusal sets the link status data_020a0f94 to 1
   (error). */
int func_0203fc38(void) {
    if (WM_SetIndCallback(func_02040384) != WM_ERRCODE_SUCCESS) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
