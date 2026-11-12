#include "types.h"
#include "nitro/wm/wm.h"

extern unsigned short data_020a0f2c;
extern void func_02040634(void *arg);
extern int data_020a0f94;
extern WMErrCode func_0206259c(WMCallbackFunc callback, u32 param);
/* Open (data_020a0f2c 1) or close (0) the parent to new children:
   func_0206259c sends WM command 0x23 with that flag, MP-parent state only,
   and func_02040634 (empty) as the callback. func_0203ea5c closes it when
   every child slot is taken and reopens it when one frees, which is what
   the SDK's WM_SetEntry does. As func_0203fc74 otherwise. */
int func_0203fd64(void) {
    if (func_0206259c(func_02040634, data_020a0f2c) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
