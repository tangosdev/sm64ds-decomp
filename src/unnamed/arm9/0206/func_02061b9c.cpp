//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

extern int *WM_GetSystemWork(void);
extern int WM_CheckStateEx(int a, int b, int c);

/* NitroSDK WM_GetMPSendBufferSize(): 0 unless the system is a parent or a
 * child (states 7, 8) that has not started MP yet; otherwise mp_maxSendSize
 * rounded up to 32 bytes. The WMArm9Buf / WMStatus view follows
 * func_02061ab0, the receive-size sibling that reads mp_flag the same way and
 * mp_maxRecvSize at 0x190. */
typedef struct WMStatus {
    char pad_000[0x10];
    int mp_flag; /* 0x010 -- 1 once MP has started */
    char pad_014[0x17a];
    u16 mp_maxSendSize; /* 0x18e */
    u16 mp_maxRecvSize; /* 0x190 */
} WMStatus;

typedef char WMStatus_size_must_be_0x194[sizeof(WMStatus) == 0x194 ? 1 : -1];

typedef struct WMArm9Buf {
    char pad_00[4];
    WMStatus *status; /* 0x04 */
} WMArm9Buf;

u32 func_02061b9c(void)
{
    WMArm9Buf *r4;
    WMStatus *p;

    r4 = (WMArm9Buf *)WM_GetSystemWork();
    if (WM_CheckStateEx(2, 7, 8) != 0)
        return 0;

    CP15::InvalidateDataCache((u32)&r4->status->mp_flag, 4);

    p = r4->status;
    if (p->mp_flag == 1)
        return 0;

    CP15::InvalidateDataCache((u32)&p->mp_maxSendSize, 4);

    return (r4->status->mp_maxSendSize + 0x1f) & ~0x1f;
}
}
