//cpp
#include "types.h"
#include "CP15.h"
#include "IRQ.h"

extern "C" {

#pragma opt_common_subs off
extern int WM_GetSystemWork(void);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern int func_020623ec(int a0, int a1, int a2, int a3, u16 a4, u16 a5);
extern void func_02062d10(void *self);

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

typedef char WMDataSharingInfo_size_must_be_0x420[sizeof(WMDataSharingInfo) == 0x420 ? 1 : -1];

void func_020627e8(u8 *self, int flag)
{
    int ctx;
    u32 saved;
    u16 slot;
    u16 next;
    u16 mask;
    u16 prevSlot;
    u16 newSlot;
    volatile u16 zero;
    int ret;
    WMDataSharingInfo *info = (WMDataSharingInfo *)self;

    ctx = WM_GetSystemWork();
    saved = IRQ::Disable();
    slot = info->writeIndex;
    if (info->ds[slot].aidHeader == 0) {
        CP15::InvalidateDataCache(*(u32 *)(ctx + 4) + 0x86, 2);

        prevSlot = info->writeIndex;
        next = (u16)((prevSlot + 1) % 4);
        mask = *(u16 *)(*(u32 *)(ctx + 4) + 0x86);
        newSlot = (info->doubleMode == 1) ? (u16)((next + 1) % 4) : next;

        zero = 0;
        MultiStore16(zero, &info->ds[newSlot], 0x100);

        info->ds[newSlot].aidHeader = (u16)(info->aidBitmap & (mask | 1));
        info->writeIndex = next;
        info->ds[prevSlot].aidHeader = info->aidBitmap;
        if (flag == 1) {
            info->ds[prevSlot].aidHeader &= ~1;
        }

        IRQ::Restore(saved);

        ret = func_020623ec((int)func_02062d10, (int)&info->ds[prevSlot], info->dataSetLength,
                            (u16)(info->aidBitmap & mask), info->port, 1);

        if (ret == 7) {
            info->seqNum[prevSlot] = 0xffff;
            info->sendIndex = (u16)((info->sendIndex + 1) % 4);
        } else if (ret != 0 && ret != 2) {
            info->state = 3;
        }
    } else {
        IRQ::Restore(saved);
    }
}
}
