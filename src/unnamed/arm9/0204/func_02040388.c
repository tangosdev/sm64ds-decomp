#include "nitro/wm/wm.h"

extern int func_0203fcec(void);
extern int func_0203fcb0(void);
extern int data_020a0f5c;
extern int data_020a0f94;

/* The callback func_0203fc74 hands WM_PowerOff. On success, for target mode data_020a0f5c 0 disable the
   hardware (func_0203fcec); for 3 or 4 power it on again (func_0203fcb0).
   An error sets the link status data_020a0f94 to 1. */
void func_02040388(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    int s;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        s = data_020a0f5c;
        switch (s) {
        case 0:
            func_0203fcec();
            break;
        case 3:
        case 4:
            func_0203fcb0();
            break;
        }
        return;
    }
    data_020a0f94 = 1;
}
