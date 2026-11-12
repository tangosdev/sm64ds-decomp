#include "types.h"
#include "nitro/wm/wm_api.h"
#include "private/wm_views.h"
extern int data_020a0f5c;
extern u16 data_020a0f24;
extern u8 data_020a0f9d[4];
extern u16 data_020a0fbe[4][3];
extern u16 data_020a0fa0[][3];

extern int Wireless_Reset(void);
extern int func_0203faa8(void);
extern void MI_CpuCopy8(const void *src, void *dst, unsigned int size);
extern int func_02042748(u16 *a, u16 *b);

/* The parent's WM_StartParent callback. A connecting child stays only when
 * its MAC address equals one of the three entries of data_020a0fbe whose
 * data_020a0f9d flag is set (func_02042748 compares the three halfwords);
 * data_020a0fa0[aid] then takes the address. Any other child is
 * disconnected (WM_Disconnect). */
void func_02040014(void *arg) {
    WmStartParentCallback *cb = (WmStartParentCallback *)arg;
    int v;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        v = cb->state;
        switch (v) {
        case WM_STATECODE_PARENT_START: {
            int sel = data_020a0f5c;
            data_020a0f24 = 0;
            switch (sel) {
            case 0:
                Wireless_Reset();
                break;
            case 3:
                if (func_0203faa8() != 0)
                    return;
                data_020a0f5c = 0;
                Wireless_Reset();
                break;
            case 4:
                data_020a0f5c = 4;
                Wireless_Reset();
                break;
            }
            break;
        }
        case WM_STATECODE_BEACON_SENT:
            break;
        case WM_STATECODE_CONNECTED: {
            u16 buf[3];
            int matched;
            int cnt;
            u8 *flag_p;
            u16 *item_p;

            matched = 0;
            MI_CpuCopy8(cb->macAddress, buf, 6);

            flag_p = data_020a0f9d;
            item_p = data_020a0fbe[0];
            cnt = 1;
            do {
                if (*flag_p != 0) {
                    if (func_02042748(buf, item_p) != 0) {
                        u16 idx = cb->aid;
                        MI_CpuCopy8(buf, data_020a0fa0[idx], 6);
                        matched = 1;
                        break;
                    }
                }
                cnt++;
                flag_p++;
                item_p += 3;
            } while (cnt < 4);

            /* matched kept: an int-returning inline search predicate re-colours the loop */
            if (matched != 0) {
                return;
            }
            WM_Disconnect(0, cb->aid);
            break;
        }
        case WM_STATECODE_DISCONNECTED:
            break;
        }
    } else {
        data_020a0f5c = 0;
        Wireless_Reset();
    }
}
