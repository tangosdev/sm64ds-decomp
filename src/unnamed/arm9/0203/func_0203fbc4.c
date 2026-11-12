#include "nitro/wm/wm_api.h"

extern void func_020401e8(unsigned short *cb);
extern WMParentParam *data_020a0f44;
/* Set the parent parameters: WM_SetParentParameter on the
   WMParentParam in data_020a0f44, with func_020401e8 as the callback, which
   starts the parent (func_0203fb98). 1 when the request is accepted (2). */
int func_0203fbc4(void) {
    return WM_SetParentParameter((WMCallbackFunc)func_020401e8, data_020a0f44) ==
           WM_ERRCODE_OPERATING;
}
