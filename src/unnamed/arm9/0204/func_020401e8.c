#include "nitro/wm/wm.h"

extern int Wireless_Reset(void);
extern int func_0203fb98(void);

extern int data_020a0f5c;

/* The callback func_0203fbc4 hands WM_SetParentParameter. On success, for target mode data_020a0f5c 3 (parent) start
   the parent (func_0203fb98); if that is refused, clear the mode and
   reset (Wireless_Reset). For 0 and 4 (child) just reset. An error
   clears the mode and resets. */
void func_020401e8(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        switch (data_020a0f5c) {
        case 0:
            Wireless_Reset();
            break;
        case 3:
            if (func_0203fb98() != 0) {
                return;
            }
            data_020a0f5c = 0;
            Wireless_Reset();
            break;
        case 4:
            data_020a0f5c = 4;
            Wireless_Reset();
            break;
        }
    } else {
        data_020a0f5c = 0;
        Wireless_Reset();
    }
}
