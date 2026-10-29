//cpp
#include "IRQ.h"

extern "C" {

extern void *func_02062734(void *c, unsigned m, void *acc, int bit);
extern void MultiCopyHalf(void *dst, void *src, int n);
extern void MultiStore16(unsigned short val, void *dst, int n);

/* NitroSDK data sharing, WmDataSharingReceiveData: store the data AID bit
 * sent into the data set still waiting for it (ds[writeIndex], or in double
 * mode the next one), at that member's slot (func_02062734 steps dataLength
 * per member before it), or zero the slot when there is no data. The bit then
 * moves from the set's aidHeader to its receivedBitmap. Called by the parent's
 * port receive callback (func_02062bdc) and by the data sharing step
 * (func_02062df0) for the parent's own data. */

/* NitroSDK WMDataSet / WMDataSharingInfo, as func_02062aa4 spells them. */
typedef struct WMDataSet {
    unsigned short aidHeader;      /* 0x00 -- members whose data the set holds */
    unsigned short receivedBitmap; /* 0x02 */
    unsigned short data[0x7e];     /* 0x04 */
} WMDataSet;

typedef struct WMDataSharingInfo {
    WMDataSet ds[4];              /* 0x000 */
    unsigned short seqNum[4];     /* 0x400 -- each set's MP sequence number */
    unsigned short writeIndex;    /* 0x408 */
    unsigned short sendIndex;     /* 0x40a */
    unsigned short readIndex;     /* 0x40c */
    unsigned short aidBitmap;     /* 0x40e -- the members, this station included */
    unsigned short dataLength;    /* 0x410 -- per member */
    unsigned short stationNumber; /* 0x412 */
    unsigned short dataSetLength; /* 0x414 -- dataLength * stationNumber + 4 */
    unsigned short port;          /* 0x416 */
    unsigned short doubleMode;    /* 0x418 */
    unsigned short currentSeqNum; /* 0x41a */
    unsigned short state;         /* 0x41c -- 1 running, 2 retry the send, 3 error */
    unsigned short reserved[1];   /* 0x41e */
} WMDataSharingInfo;

#pragma opt_common_subs off
void func_02062990(char *c, int bit, void *arg2)
{
    WMDataSharingInfo *info = (WMDataSharingInfo *)c;
    unsigned short mask = (unsigned short)(1 << bit);
    unsigned short v40e = info->aidBitmap;
    unsigned short idx;
    volatile unsigned short zero;
    void *r;
    unsigned s;

    if ((v40e & mask) == 0) return;

    idx = info->writeIndex;
    if ((mask & info->ds[idx].aidHeader) == 0) {
        if (info->doubleMode != 1)
            return;
        idx = (idx + 1) % 4;
        if ((mask & info->ds[idx].aidHeader) == 0)
            return;
    }

    r = func_02062734(c, v40e, info->ds[idx].data, bit);
    if (arg2 != 0) {
        MultiCopyHalf(arg2, r, info->dataLength);
    } else {
        zero = 0;
        MultiStore16(zero, r, info->dataLength);
    }
    s = IRQ::Disable();
    info->ds[idx].aidHeader &= ~mask;
    info->ds[idx].receivedBitmap |= mask;
    IRQ::Restore(s);
}
}
