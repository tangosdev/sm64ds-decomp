//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

#pragma opt_common_subs off
#pragma opt_strength_reduction off
extern int WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, ...);
extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern int func_02061c88(int idx, int b, int c);
typedef void (*Fn)(void *);
extern int func_020623ec(Fn a0, int a1, int a2, int a3, u16 a4, u16 a5);
extern void func_02062bdc(void);
extern void func_02062d10(void *self);
extern void func_02062aa4(void);

/* NitroSDK WMDataSet / WMDataSharingInfo, as func_02062aa4 spells them. */
typedef struct WMDataSet {
    u16 aidHeader;      /* 0x00 -- members whose data the set holds */
    u16 receivedBitmap; /* 0x02 */
    u16 data[0x7e];     /* 0x04 */
} WMDataSet;

typedef struct WMDataSharingInfo {
    WMDataSet ds[4];   /* 0x000 */
    u16 seqNum[4];     /* 0x400 -- each set's MP sequence number */
    u16 writeIndex;    /* 0x408 */
    u16 sendIndex;     /* 0x40a */
    u16 readIndex;     /* 0x40c */
    u16 aidBitmap;     /* 0x40e -- the members, this station included */
    u16 dataLength;    /* 0x410 -- per member */
    u16 stationNumber; /* 0x412 */
    u16 dataSetLength; /* 0x414 -- dataLength * stationNumber + 4 */
    u16 port;          /* 0x416 */
    u16 doubleMode;    /* 0x418 */
    u16 currentSeqNum; /* 0x41a */
    u16 state;         /* 0x41c -- 1 running, 2 retry the send, 3 error */
    u16 reserved[1];   /* 0x41e */
} WMDataSharingInfo;

/* WM_GetSystemWork's WMArm9Buf and its WMStatus, as func_02061ab0 views
 * them. 0x86 is a bitmap of the children the parent's data sets wait for
 * (func_020627e8 masks aidBitmap with it the same way); it has no name in
 * any view yet, so it stays padding here and is read by offset below. */
typedef struct WMStatus {
    char pad_000[0x10];
    int mp_flag; /* 0x010 -- 1 once MP has started */
    char pad_014[0x184 - 0x14];
    u16 aid; /* 0x184 -- 0 for the parent */
} WMStatus;

typedef struct WMArm9Buf {
    char pad_00[4];
    WMStatus *status; /* 0x04 */
} WMArm9Buf;

/* NitroSDK WM_StartDataSharing(dsInfo, port, aidBitmap, dataLength,
 * doubleMode): with MP running, clear the sharing info, add this station's
 * own AID bit to aidBitmap and size a data set as dataLength per member
 * plus the 4-byte header (at most 0xfc before the header). The parent
 * (aid 0) primes the four sets, registers its port receive callback
 * (func_02062bdc) and sends the first set (two in double mode); a child
 * registers func_02062aa4 and waits. */
int func_020631dc(u8 *self, int port, int aidBitmap, int dataLength, int doubleMode) {
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)self;
    u16 childMask = 1;
    WMArm9Buf *sys;
    int r;
    u16 aid;
    u16 cnt;
    u16 j;
    u16 mbit;
    u16 bits;
    volatile unsigned zero;

    sys = (WMArm9Buf *)WM_GetSystemWork();
    r = WM_CheckStateEx(2, 9, 0xa);
    if (r != 0)
        return r;

    CP15::InvalidateDataCache((u32)&sys->status->mp_flag, 4);
    if (sys->status->mp_flag == 0)
        return 3;

    if (self == 0)
        return 6;
    if ((u32)port >= 0x10)
        return 6;
    if (aidBitmap == 0)
        return 6;

    CP15::InvalidateDataCache((u32)&sys->status->aid, 2);
    aid = sys->status->aid;
    if (aid == 0) {
        CP15::InvalidateDataCache((u32)sys->status + 0x86, 2);
        childMask = *(u16 *)((u32)sys->status + 0x86);
    }

    zero = 0;
    MultiStore32Bytes(zero, (int *)self, 0x420);

    dsInfo->writeIndex = 0;
    dsInfo->sendIndex = 0;
    dsInfo->readIndex = 0;
    dsInfo->dataLength = dataLength;
    dsInfo->port = port;
    bits = 0;
    dsInfo->aidBitmap = bits;

    dsInfo->doubleMode = (doubleMode != 0);
    mbit = 1;
    bits = (u16)(aidBitmap | (mbit << aid));
    dsInfo->aidBitmap = bits;

    cnt = 0;
    for (j = 0; j < 0x10; j++) {
        if (bits & mbit)
            cnt++;
        mbit = (u16)(mbit << 1);
    }

    dsInfo->stationNumber = cnt;
    dsInfo->dataSetLength = (u16)(dataLength * cnt);
    if (dsInfo->dataSetLength > 0xfc) {
        dsInfo->aidBitmap = 0;
        return 6;
    }

    dsInfo->dataSetLength += 4;
    dsInfo->state = 1;

    if (aid == 0) {
        int i;
        WMDataSet *cur;
        int n;

        for (i = 0; i < 4; i++)
            dsInfo->ds[i].aidHeader = (u16)(dsInfo->aidBitmap & (childMask | 1));

        func_02061c88(port, (int)func_02062bdc, (int)self);

        cur = dsInfo->ds;
        for (n = 0; n < ((dsInfo->doubleMode == 1) ? 2 : 1); n++) {
            int cr;

            dsInfo->writeIndex = (dsInfo->writeIndex + 1) % 4;
            cr = func_020623ec(func_02062d10, (int)cur, dsInfo->dataSetLength,
                               (u16)(dsInfo->aidBitmap & childMask), dsInfo->port, 1);

            if (cr == 7) {
                dsInfo->seqNum[n] = 0xffff;
                dsInfo->sendIndex = (dsInfo->sendIndex + 1) % 4;
            } else if (cr != 0 && cr != 2) {
                dsInfo->state = 3;
                return 1;
            }

            cur++;
        }
    } else {
        dsInfo->sendIndex = 3;
        func_02061c88(port, (int)func_02062aa4, (int)self);
    }

    return 0;
}
}
