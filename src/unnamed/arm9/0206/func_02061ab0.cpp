//cpp
#include "CP15.h"

extern "C" {

extern int WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, ...);

/* NitroSDK WM_GetMPReceiveBufferSize(): 0 unless the system is a parent or a
 * child (states 7, 8) that has not started MP yet (mp_flag); otherwise the
 * receive-buffer size for the per-packet receive size mp_maxRecvSize: as a
 * parent (aid 0) for pparam.maxEntry children, as a child for one parent.
 * Each status field is invalidated from the data cache before it is read.
 * Field names follow the SDK's WMArm9Buf and WMStatus; only the fields this
 * function reads are spelled out. pparam is WMParentParam in this ROM's
 * layout (maxEntry at +0x0e, see include/WmWork.h's WmParentParam). */
typedef struct WMStatus {
    char pad_000[0x10];
    int mp_flag; /* 0x010 -- 1 once MP has started */
    char pad_014[0xd0];
    struct {
        char pad_00[0xe];
        unsigned short maxEntry; /* 0x0f2 */
    } pparam;                    /* 0x0e4 */
    char pad_0f4[0x90];
    unsigned short aid; /* 0x184 -- 0 for the parent */
    char pad_186[0xa];
    unsigned short mp_maxRecvSize; /* 0x190 */
} WMStatus;

typedef char WMStatus_view_size_must_be_0x194[sizeof(WMStatus) == 0x194 ? 1 : -1];

typedef struct WMArm9Buf {
    char pad_00[4];
    WMStatus *status; /* 0x04 */
} WMArm9Buf;

int func_02061ab0(void) {
    WMArm9Buf *h = (WMArm9Buf *)WM_GetSystemWork();
    WMStatus *p;
    int flag;
    int v;

    if (WM_CheckStateEx(2, 7, 8) != 0)
        return 0;

    CP15::InvalidateDataCache((unsigned int)&h->status->mp_flag, 4);
    p = h->status;
    if (p->mp_flag == 1)
        return 0;

    CP15::InvalidateDataCache((unsigned int)&h->status->aid, 2);
    p = h->status;
    flag = (p->aid == 0);

    CP15::InvalidateDataCache((unsigned int)&p->mp_maxRecvSize, 2);
    p = h->status;
    v = p->mp_maxRecvSize;
    if (flag != 1)
        return (((v + 0x51) & ~0x1f) << 1);

    CP15::InvalidateDataCache((unsigned int)&p->pparam.maxEntry, 2);
    p = h->status;
    return (((((v + 0xc) * p->pparam.maxEntry) + 0x29) & ~0x1f) << 1);
}
}
