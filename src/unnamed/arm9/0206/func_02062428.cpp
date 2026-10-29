//cpp
/* func_02062428 at 0x02062428
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
#include "CP15.h"

extern "C" {

extern int *WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, ...);
extern int WM_SendCommand(int a, int b, int c, int d, int e, int f, int g, int h, int i);

/* WM_GetSystemWork's WMArm9Buf and its WMStatus, the fields read here.
 * child_bitmap follows func_020618b8 and include/WmWork.h; mp_maxSendSize
 * is the SDK field just before func_02061ab0's mp_maxRecvSize (0x190), and
 * func_02061b9c rounds it up for the send-buffer size. 0x86 has no name in
 * any view yet, so it is only invalidated by offset below. */
typedef struct WMStatus {
    char pad_000[0x17e];
    unsigned short child_bitmap; /* 0x17e -- connected children, by AID bit */
    char pad_180[0x184 - 0x180];
    unsigned short aid; /* 0x184 -- 0 for the parent */
    char pad_186[0x18e - 0x186];
    unsigned short mp_maxSendSize; /* 0x18e */
} WMStatus;

typedef struct WMArm9Buf {
    char pad_00[4];
    WMStatus *status; /* 0x04 */
} WMArm9Buf;

int func_02062428(int p0, int p1, int p2, int p3, unsigned short p4, unsigned short p5,
                  unsigned short p6) {
    int maxSendSize;
    int x;
    WMArm9Buf *handle;
    int ret;
    int aid;
    int flag;

    x = 1;
    handle = (WMArm9Buf *)WM_GetSystemWork();
    ret = WM_CheckStateEx(2, 9, 0xa);
    if (ret != 0)
        return ret;

    CP15::InvalidateDataCache((unsigned int)&handle->status->mp_maxSendSize, 2);
    maxSendSize = handle->status->mp_maxSendSize;
    CP15::InvalidateDataCache((unsigned int)&handle->status->aid, 2);

    aid = handle->status->aid;
    flag = (aid == 0) ? x : 0;
    if (flag == 1) {
        CP15::InvalidateDataCache((unsigned int)&handle->status->child_bitmap, 2);
        x = handle->status->child_bitmap;
        CP15::InvalidateDataCache((unsigned int)handle->status + 0x86, 2);
    }

    if (p2 == 0)
        return 6;
    if (x == 0)
        return 7;
    if (p3 + (flag != 0 ? 4 : 2) > maxSendSize)
        return 6;
    if (p3 == 0)
        return 6;

    CP15::FlushDataCache(p2, p3);
    ret = WM_SendCommand(0xf, 7, p2, p3, p4, p5, p6, p0, p1);
    if (ret == 0)
        return 2;
    return ret;
}
}
