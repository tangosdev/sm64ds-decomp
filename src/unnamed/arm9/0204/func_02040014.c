#include "types.h"
extern int data_020a0f5c;
extern u16 data_020a0f24;
extern u8 data_020a0f9d[4];
extern u16 data_020a0fbe[4][3];
extern u16 data_020a0fa0[][3];

extern int Wireless_Reset(void);
extern int func_0203faa8(void);
extern void CpuCopy8(void *dst, void *src, int n);
extern int func_02042748(u16 *a, u16 *b);
extern int func_02061d30(int self, unsigned int idx);

/* Leading fields of NitroSDK WMStartParentCallback, with the names
 * func_02066a94.c gives them. */
typedef struct WmStartParentCallback {
    u16 apiid;        /* 0x00 */
    u16 errcode;      /* 0x02 */
    u16 wlCmdID;      /* 0x04 */
    u16 wlResult;     /* 0x06 */
    u16 state;        /* 0x08 -- 0 PARENT_START, 2 BEACON_SENT, 7 CONNECTED, 9 DISCONNECTED */
    u8 macAddress[6]; /* 0x0a -- the child's */
    u16 aid;          /* 0x10 */
} WmStartParentCallback;

/* The parent's WM_StartParent callback. A connecting child stays only when
 * its MAC address equals one of the three entries of data_020a0fbe whose
 * data_020a0f9d flag is set (func_02042748 compares the three halfwords);
 * data_020a0fa0[aid] then takes the address. Any other child is
 * disconnected (func_02061d30, WM_Disconnect). The parameter keeps its
 * char * spelling: the other declaration is checked against it. */
void func_02040014(char *a)
{
    WmStartParentCallback *cb = (WmStartParentCallback *)a;
    int v;

    if (cb->errcode == 0) {
        v = cb->state;
        switch (v) {
        case 0: {
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
        case 2:
            break;
        case 7: {
            u16 buf[3];
            int matched;
            int cnt;
            u8 *flag_p;
            u16 *item_p;

            matched = 0;
            CpuCopy8(cb->macAddress, buf, 6);

            flag_p = data_020a0f9d;
            item_p = data_020a0fbe[0];
            cnt = 1;
            do {
                if (*flag_p != 0) {
                    if (func_02042748(buf, item_p) != 0) {
                        u16 idx = cb->aid;
                        CpuCopy8(buf, data_020a0fa0[idx], 6);
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
            func_02061d30(0, cb->aid);
            break;
        }
        case 9:
            break;
        }
    } else {
        data_020a0f5c = 0;
        Wireless_Reset();
    }
}
