//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

#pragma opt_common_subs off
extern int WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, ...);
extern int func_020623ec(int a0, int a1, int a2, int a3, u16 a4, u16 a5);
extern void MultiCopyHalf(const void *src, void *dst, u32 count);
extern void func_020627e8(void *self, int x);
extern void func_02062990(void *self, u32 a, u32 b);
extern void func_02062d10(void *self);

/* NitroSDK WM_StepDataSharing(dsInfo, sendData, receiveData): one frame of
 * data sharing. The parent (aid 0) resends a set the last send refused,
 * hands the oldest complete set to receiveData, then merges its own
 * sendData into the set being filled (func_02062990) and sends whatever is
 * ready (func_020627e8). A child arms or hands over the received set and
 * sends its own data to the parent. Returns 0 with a set handed over, 5
 * when none was ready, 1 on a send error. */

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

/* WM_GetSystemWork's WMArm9Buf and its WMStatus, as func_020631dc views
 * them. 0x86 (the children the parent's sets wait for) has no name in any
 * view yet, so it is read by offset below. */
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

int func_02062df0(u8 *self, u8 *sendData, u8 *receiveData) {
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)self;
    /* the system work, then the return code: a separate WMArm9Buf * local
     * recolours the registers */
    int result;
    u16 aid;
    u16 childMask;
    int did;
    int fp;
    int state;
    int r;

    result = WM_GetSystemWork();
    r = WM_CheckStateEx(2, 9, 0xa);
    if (r != 0)
        return r;

    CP15::InvalidateDataCache((u32)(&((WMArm9Buf *)result)->status->mp_flag), 4);
    if (((WMArm9Buf *)result)->status->mp_flag == 0)
        return 3;

    if (self == 0)
        return 6;
    if (sendData == 0)
        return 6;
    if (receiveData == 0)
        return 6;

    CP15::InvalidateDataCache((u32)(&((WMArm9Buf *)result)->status->aid), 2);
    aid = ((WMArm9Buf *)result)->status->aid;
    if (aid == 0) {
        CP15::InvalidateDataCache((u32)((WMArm9Buf *)result)->status + 0x86, 2);
        childMask = *(u16 *)((u32)((WMArm9Buf *)result)->status + 0x86);
    }

    state = dsInfo->state;
    if (state == 3)
        return 1;
    if (state != 1 && state != 2)
        return 3;

    result = 5;

    if (aid == 0) {
        did = 0;
        fp = did;

        if (state == 2) {
            u16 n;
            int cr;
            dsInfo->state = 1;
            n = (u16)((dsInfo->writeIndex + 3) % 4);
            cr = func_020623ec((int)func_02062d10, (int)&dsInfo->ds[n], dsInfo->dataSetLength,
                               (u16)(dsInfo->aidBitmap & childMask), dsInfo->port, 1);
            if (cr == 7) {
                dsInfo->seqNum[n] = 0xffff;
                dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % 4);
            } else if (cr != 0 && cr != 2) {
                dsInfo->state = 3;
                return 1;
            }
        }

        if (dsInfo->readIndex != dsInfo->sendIndex) {
            dsInfo->ds[dsInfo->readIndex].aidHeader |= 1;
            MultiCopyHalf(&dsInfo->ds[dsInfo->readIndex], receiveData, 0x100);
            did = 1;
            result = 0;
            dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
            dsInfo->readIndex = (u16)((dsInfo->readIndex + 1) % 4);

            if (dsInfo->doubleMode == 0 && childMask != 0 &&
                dsInfo->ds[dsInfo->writeIndex].aidHeader == 1)
                fp = did;
            else
                fp = 0;
        }

        func_020627e8(self, 0);
        if (did != 0) {
            func_02062990(self, 0, (u32)sendData);
            if (dsInfo->doubleMode == 0)
                func_020627e8(self, fp);
        }
    } else {
        int changed;

        changed = 0;
        if (state == 2) {
            dsInfo->state = 1;
            changed = 1;
        } else {
            if (dsInfo->readIndex != dsInfo->writeIndex) {
                if ((dsInfo->ds[dsInfo->readIndex].aidHeader & 1) == 0) {
                    dsInfo->ds[dsInfo->readIndex].aidHeader |= 1;
                } else {
                    MultiCopyHalf(&dsInfo->ds[dsInfo->readIndex], receiveData, 0x100);
                    changed = 1;
                    result = 0;
                    dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
                    dsInfo->readIndex = (u16)((dsInfo->readIndex + 1) % 4);
                }
            }
        }

        if (changed != 0) {
            /* the child stages its own data 0x20 bytes into the set it sends */
            u8 *addr = (u8 *)&dsInfo->ds[dsInfo->sendIndex] + 0x20;
            int cr;
            MultiCopyHalf(sendData, addr, dsInfo->dataLength);
            cr = func_020623ec((int)func_02062d10, (int)addr, dsInfo->dataLength, dsInfo->aidBitmap,
                               dsInfo->port, 1);
            dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % 4);
            if (cr != 2 && cr != 0) {
                dsInfo->state = 3;
                result = 1;
            }
        }
    }

    return result;
}
}
