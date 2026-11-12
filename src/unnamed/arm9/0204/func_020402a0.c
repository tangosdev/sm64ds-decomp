#include "nitro/wm/wm.h"

extern int func_0203fc74(void);
extern void func_02040638(void);
extern int func_0203fbc4(void);
extern int Wireless_Reset(void);
extern int func_0203fb5c(void);

extern int data_020a0f5c;
extern unsigned short data_020a0f24;
extern int data_020a0f94;

/* The callback Wireless_Reset hands WM_Reset (command 1).
   On success clear the AID data_020a0f24 and go on by target mode
   data_020a0f5c: 0 powers off (func_0203fc74); 3 sets the parent up
   again (func_02040638, then func_0203fbc4); 4 connects to the parent
   again (func_0203fb5c). A refused request clears the mode and resets.
   An error sets the link status data_020a0f94 to 1. */
void func_020402a0(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    int x;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        x = data_020a0f5c;
        data_020a0f24 = 0;
        switch (x) {
        case 0:
            func_0203fc74();
            return;
        case 3:
            func_02040638();
            if (func_0203fbc4() != 0)
                return;
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        case 4:
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
