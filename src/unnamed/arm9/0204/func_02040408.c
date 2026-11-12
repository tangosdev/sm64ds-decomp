#include "nitro/wm/wm.h"

extern int data_020a0f5c;
extern int data_020a0f94;
extern int func_0203fc74(void);
extern int func_0203fc38(void);
extern void func_02040638(void);
extern int func_0203fbc4(void);
extern int Wireless_Reset(void);
extern int func_0203fb5c(void);

/* The callback func_0203fcb0 hands WM_PowerOn. On success, for target mode data_020a0f5c 0 power off again
   (func_0203fc74). For 3 (parent), install the indication callback
   (func_0203fc38), then set the parent up (func_02040638,
   func_0203fbc4); for 4 (child), install it and connect
   (func_0203fb5c). A refused request clears the mode and resets
   (Wireless_Reset). An error sets the link status data_020a0f94 to 1. */
void func_02040408(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        switch (data_020a0f5c) {
        case 0:
            func_0203fc74();
            return;
        case 3:
            if (func_0203fc38() == 0)
                return;
            func_02040638();
            if (func_0203fbc4() != 0)
                return;
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        case 4:
            if (func_0203fc38() != 1)
                return;
            if (func_0203fb5c() != 0)
                return;
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        }
    } else {
        data_020a0f94 = 1;
    }
}
