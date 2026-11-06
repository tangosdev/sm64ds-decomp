#include "WmWork.h"

extern int data_020a0f5c;
extern int data_020a0f94;
extern unsigned short data_020a0f24;
extern int Wireless_Reset(void);
extern int func_0203faa8(void);

/* The callback func_0203fb5c hands WM_StartConnect. An
   error clears data_020a0f5c and resets the link (Wireless_Reset). On
   success it resets when data_020a0f5c is 0 or 3, returns when it is
   anything but 4, and at 4 follows the state:
     CONNECTED (7): keep the AID in data_020a0f24 (func_02040704 returns
       it); unless data_020a0f94 is 4, start MP (func_0203faa8 returns 1
       when WM_StartMP is accepted) and clear data_020a0f5c and reset if not.
     DISCONNECTED (9): clear data_020a0f5c; reset unless data_020a0f94 is 2.
     CONNECT_START (6), BEACON_LOST (8): nothing.
     anything else: clear data_020a0f5c and reset. */
void func_0203fec4(WmStartConnectCallback *p) {
    int g;
    if (p->errcode == 0) {
        g = data_020a0f5c;
        switch (g) {
        case 0:
        case 3:
            Wireless_Reset();
            return;
        case 4:
            switch (p->state) {
            case 7:
                data_020a0f24 = p->aid;
                if (data_020a0f94 == 4)
                    return;
                if (func_0203faa8() != 0)
                    return;
                data_020a0f5c = 0;
                Wireless_Reset();
                return;
            case 9:
                data_020a0f5c = 0;
                if (data_020a0f94 == 2)
                    return;
                Wireless_Reset();
                return;
            case 6:
            case 8:
                break;
            default:
                data_020a0f5c = 0;
                Wireless_Reset();
                return;
            }
            break;
        default:
            return;
        }
    } else {
        data_020a0f5c = 0;
        Wireless_Reset();
    }
}
