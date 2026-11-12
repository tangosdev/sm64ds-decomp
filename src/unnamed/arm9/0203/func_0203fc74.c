#include "nitro/wm/wm_api.h"

extern void func_02040388(unsigned short *cb);
extern int data_020a0f94;

/* Power the wireless hardware off: WM_PowerOff sends WM command 6 (the
   SDK's WM_APIID_POWER_OFF, idle state only) with func_02040388 as the
   callback. A refusal sets the link status data_020a0f94 to 1 (error) and
   returns 0; otherwise 1. func_0203fcb0, func_0203fcec, func_0203fd28
   and func_0203fd64 are the same wrapper on other commands.
   Wireless_Reset is close: it sends WM command 1 (WM_Reset)
   but shuts the file share down (func_02040aa4) before it checks the
   result. */
int func_0203fc74(void) {
    if (WM_PowerOff((WMCallbackFunc)func_02040388) != WM_ERRCODE_OPERATING) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
