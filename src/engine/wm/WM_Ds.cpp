//cpp
/* NitroSDK wm_ds.c: WM data sharing. Every MP frame the parent gathers one
 * data set (a 4-byte header and every member's dataLength bytes in AID
 * order) and sends it to the children, who send their own data back.
 * dsInfo holds four sets in a ring; writeIndex is the set being filled,
 * sendIndex the next to send and readIndex the next WM_StepDataSharing
 * hands over. This SDK is older than pret's pokediamond: the sets go out
 * with WM_SetMPDataToPort, a failed send is state 3, not 5, and
 * WM_EndDataSharing is not linked. */
#include "types.h"
#include "CP15.h"
#include "IRQ.h"

extern "C" {

/* NitroSDK WMDataSet / WMDataSharingInfo (0x420; WM_StartDataSharing clears
 * that many bytes): four 0x100-byte data sets, then the ring indices. */
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

/* NitroSDK WMPortRecvCallback, the leading fields: the offsets agree with
 * every port receive callback in this cluster and with func_020417d0. */
typedef struct WMPortRecvCallback {
    u16 apiid;        /* 0x00 */
    u16 errcode;      /* 0x02 */
    u16 state;        /* 0x04 -- 7 CONNECTED, 9 DISCONNECTED, 0x15 PORT_RECV */
    u16 port;         /* 0x06 */
    void *recvBuf;    /* 0x08 */
    u16 *data;        /* 0x0c */
    u16 length;       /* 0x10 -- bytes at data */
    u16 aid;          /* 0x12 -- the sender */
    u8 macAddress[6]; /* 0x14 */
    u16 seqNo;        /* 0x1a */
    void *arg;        /* 0x1c -- WM_SetPortCallback's argument */
} WMPortRecvCallback;

/* NitroSDK WMPortSendCallback: only the fields read here. */
typedef struct WMPortSendCallback {
    char pad0[2];
    u16 errcode; /* 0x02 -- 0 sent, 0xa the send queue was full */
    char pad1[0xa - 0x4];
    u16 port; /* 0x0a */
    char pad2[0x1c - 0xc];
    u16 seqNo; /* 0x1c */
} WMPortSendCallback;

/* WM_GetSystemWork's WMArm9Buf and its WMStatus, as WM_GetMPReceiveBufferSize
 * views them; WM_SetPortCallback fills the per-port tables. 0x86 is a bitmap
 * of the children the parent's data sets wait for (pret's
 * WMi_GetMPReadyAIDs reads it); it has no name in any view yet, so it stays
 * padding and is read by offset. */
typedef struct WMStatus {
    char pad_000[0x10];
    int mp_flag; /* 0x010 -- 1 once MP has started */
    char pad_014[0x184 - 0x14];
    u16 aid; /* 0x184 -- 0 for the parent */
} WMStatus;

typedef struct WMArm9Buf {
    char pad_00[4];
    WMStatus *status; /* 0x04 */
    char pad_08[0xac - 0x8];
    void *portCallbackTable[16];    /* 0x0ac */
    void *portCallbackArgument[16]; /* 0x0ec */
} WMArm9Buf;

extern int WM_GetSystemWork(void);
extern int WM_CheckStateEx(int count, ...);
extern int WM_SetPortCallback(int port, int callback, int arg);
typedef void (*WMCallbackFunc)(void *arg);

/* sendData stays an int, as WM_SetMPDataToPort.c defines it. */
extern int WM_SetMPDataToPort(WMCallbackFunc callback, int sendData, int sendDataSize,
                              int destBitmap, u16 port, u16 prio);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern void MultiCopyHalf(const void *src, void *dst, int nbytes);

void WmDataSharingSetDataCallback(void *arg);
void WmDataSharingReceiveCallback_Parent(WMPortRecvCallback *callback);
void WmDataSharingReceiveCallback_Child(WMPortRecvCallback *callback);
void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, int aid, const void *data);
void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, int excludeParent);
u16 *WmGetSharedDataAddress(WMDataSharingInfo *dsInfo, unsigned aidBitmap, u16 *receiveBuf,
                            int aid);

/* NitroSDK WM_StartDataSharing(dsInfo, port, aidBitmap, dataLength,
 * doubleMode): with MP running, clear the sharing info, add this station's
 * own AID bit to aidBitmap and size a data set as dataLength per member
 * plus the 4-byte header (at most 0xfc before the header). The parent
 * (aid 0) primes the four sets, registers its port receive callback and
 * sends the first set (two in double mode); a child registers its own
 * receive callback and waits. */
#pragma opt_common_subs off
// @symbol WM_StartDataSharing
int WM_StartDataSharing(WMDataSharingInfo *dsInfo, int port, int aidBitmap, int dataLength,
                        int doubleMode) {
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

    if (dsInfo == 0)
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
    MultiStore32Bytes(zero, (int *)dsInfo, 0x420);

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

        WM_SetPortCallback(port, (int)WmDataSharingReceiveCallback_Parent, (int)dsInfo);

        cur = dsInfo->ds;
        for (n = 0; n < ((dsInfo->doubleMode == 1) ? 2 : 1); n++) {
            int cr;

            dsInfo->writeIndex = (dsInfo->writeIndex + 1) % 4;
            cr = WM_SetMPDataToPort(WmDataSharingSetDataCallback, (int)cur, dsInfo->dataSetLength,
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
        WM_SetPortCallback(port, (int)WmDataSharingReceiveCallback_Child, (int)dsInfo);
    }

    return 0;
}

/* NitroSDK WM_StepDataSharing(dsInfo, sendData, receiveData): one frame of
 * data sharing. The parent (aid 0) resends a set the last send refused,
 * hands the oldest complete set to receiveData, then merges its own
 * sendData into the set being filled (WmDataSharingReceiveData) and sends
 * whatever is ready (WmDataSharingSendDataSet). A child arms or hands over
 * the received set and sends its own data to the parent. Returns 0 with a
 * set handed over, 5 when none was ready, 1 on a send error. */
// @symbol WM_StepDataSharing
int WM_StepDataSharing(u8 *self, u8 *sendData, u8 *receiveData) {
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
            cr = WM_SetMPDataToPort(WmDataSharingSetDataCallback, (int)&dsInfo->ds[n],
                                    dsInfo->dataSetLength, (u16)(dsInfo->aidBitmap & childMask),
                                    dsInfo->port, 1);
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

        WmDataSharingSendDataSet(dsInfo, 0);
        if (did != 0) {
            WmDataSharingReceiveData(dsInfo, 0, sendData);
            if (dsInfo->doubleMode == 0)
                WmDataSharingSendDataSet(dsInfo, fp);
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
            cr = WM_SetMPDataToPort(WmDataSharingSetDataCallback, (int)addr, dsInfo->dataLength,
                                    dsInfo->aidBitmap, dsInfo->port, 1);
            dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % 4);
            if (cr != 2 && cr != 0) {
                dsInfo->state = 3;
                result = 1;
            }
        }
    }

    return result;
}
#pragma opt_common_subs on

/* The port send callback WM_StartDataSharing, WM_StepDataSharing and
 * WmDataSharingSendDataSet pass to WM_SetMPDataToPort. The sharing it
 * belongs to is the port's WM_SetPortCallback argument. On success the
 * parent records the sent set's sequence number and steps sendIndex; a full
 * send queue (errcode 0xa) backs a child's sendIndex up and asks for a
 * retry (state 2); any other failure is the error state 3. */
// @symbol WmDataSharingSetDataCallback
void WmDataSharingSetDataCallback(void *arg) {
    WMPortSendCallback *callback = (WMPortSendCallback *)arg;
    WMArm9Buf *c = (WMArm9Buf *)WM_GetSystemWork();
    WMDataSharingInfo *r4 = (WMDataSharingInfo *)c->portCallbackArgument[callback->port];
    CP15::InvalidateDataCache((u32)&c->status->aid, 2);
    u16 v = c->status->aid;
    int kind = callback->errcode;
    if (kind == 0) {
        if (v != 0)
            return;
        r4->seqNum[r4->sendIndex] = (u16)(callback->seqNo >> 1);
        r4->sendIndex = (u16)((r4->sendIndex + 1) % 4);
    } else if (kind == 0xa) {
        if (v != 0) {
            r4->sendIndex = (u16)((r4->sendIndex + 3) % 4);
        }
        r4->state = 2;
    } else {
        r4->state = 3;
    }
}

/* The parent's port receive callback. PORT_RECV (0x15) stores the child's
 * data and tries to send the next set; CONNECTED (7) just tries the send;
 * DISCONNECTED (9) stops waiting for that child in the current set (and the
 * next one in double mode) before sending. A failed receive puts the
 * sharing into state 3, the error state. */
// @symbol WmDataSharingReceiveCallback_Parent
void WmDataSharingReceiveCallback_Parent(WMPortRecvCallback *callback) {
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)callback->arg;
    if (callback->errcode == 0) {
        switch (callback->state) {
        case 0x15:
            WmDataSharingReceiveData(dsInfo, callback->aid, callback->data);
            WmDataSharingSendDataSet(dsInfo, 0);
            return;
        case 7:
            WmDataSharingSendDataSet(dsInfo, 0);
            return;
        case 9: {
            u16 mask = (u16)(1 << callback->aid);
            u32 saved = IRQ::Disable();
            u16 b408 = dsInfo->writeIndex;
            dsInfo->ds[b408].aidHeader &= ~mask;
            if (dsInfo->doubleMode == 1) {
                int n = (dsInfo->writeIndex + 1) % 4;
                dsInfo->ds[n].aidHeader &= ~mask;
            }
            IRQ::Restore(saved);
            WmDataSharingSendDataSet(dsInfo, 0);
            if (dsInfo->doubleMode == 1)
                WmDataSharingSendDataSet(dsInfo, 0);
            return;
        }
        default:
            return;
        }
    }
    dsInfo->state = 3;
}

/* The child's port receive callback. On PORT_RECV (0x15) it copies the
 * parent's whole data set into ds[writeIndex], records its sequence number
 * and steps writeIndex; CONNECTED (7) and DISCONNECTED (9) are ignored. A
 * failed receive (errcode != 0) puts the sharing into state 3. */
// @symbol WmDataSharingReceiveCallback_Child
void WmDataSharingReceiveCallback_Child(WMPortRecvCallback *callback) {
    WMArm9Buf *h = (WMArm9Buf *)WM_GetSystemWork();
    WMDataSharingInfo *r5 = (WMDataSharingInfo *)callback->arg;
    if (callback->errcode == 0) {
        u32 r4;
        u16 *src;
        {
            u16 v = callback->state;
            if (v == 7)
                return;
            if (v == 9)
                return;
            if (v != 0x15)
                return;
        }
        r4 = callback->length;
        if (r4 != r5->dataSetLength) {
            if (r4 > 0x100)
                r4 = 0x100;
        }
        CP15::InvalidateDataCache((u32)&h->status->aid, 2);
        if (r4 < 4)
            return;
        src = callback->data;
        if (*src == 0)
            return;
        if ((1u << h->status->aid) == 0)
            return;
        MultiCopyHalf(src, &r5->ds[r5->writeIndex], r4);
        r5->seqNum[r5->writeIndex] = callback->seqNo >> 1;
        r5->writeIndex = (r5->writeIndex + 1) % 4;
    } else {
        r5->state = 3;
    }
}

/* Store the data member aid sent into the data set still waiting for it
 * (ds[writeIndex], or in double mode the next one), at that member's slot,
 * or zero the slot when there is no data. The bit then moves from the set's
 * aidHeader to its receivedBitmap. */
// @symbol WmDataSharingReceiveData
void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, int aid, const void *data) {
    u16 mask = (u16)(1 << aid);
    u16 aidBitmap = dsInfo->aidBitmap;
    u16 idx;
    volatile u16 zero;
    u16 *slot;
    unsigned s;

    if ((aidBitmap & mask) == 0)
        return;

    idx = dsInfo->writeIndex;
    if ((mask & dsInfo->ds[idx].aidHeader) == 0) {
        if (dsInfo->doubleMode != 1)
            return;
        idx = (idx + 1) % 4;
        if ((mask & dsInfo->ds[idx].aidHeader) == 0)
            return;
    }

    slot = WmGetSharedDataAddress(dsInfo, aidBitmap, dsInfo->ds[idx].data, aid);
    if (data != 0) {
        MultiCopyHalf(data, slot, dsInfo->dataLength);
    } else {
        zero = 0;
        MultiStore16(zero, slot, dsInfo->dataLength);
    }
    s = IRQ::Disable();
    dsInfo->ds[idx].aidHeader &= ~mask;
    dsInfo->ds[idx].receivedBitmap |= mask;
    IRQ::Restore(s);
}

/* Once every member has filled ds[writeIndex], step writeIndex, open the
 * next set (the one after it in double mode) for the ready children and the
 * parent, and send the full set to the children. excludeParent clears the
 * parent's own bit from the sent header. With no child to send to (7) the
 * set counts as sent; any failure but OPERATING (2) is state 3. */
// @symbol WmDataSharingSendDataSet
void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, int excludeParent) {
    int ctx;
    u32 saved;
    u16 slot;
    u16 next;
    u16 mask;
    u16 prevSlot;
    u16 newSlot;
    volatile u16 zero;
    int ret;

    ctx = WM_GetSystemWork();
    saved = IRQ::Disable();
    slot = dsInfo->writeIndex;
    if (dsInfo->ds[slot].aidHeader == 0) {
        CP15::InvalidateDataCache(*(u32 *)(ctx + 4) + 0x86, 2);

        prevSlot = dsInfo->writeIndex;
        next = (u16)((prevSlot + 1) % 4);
        mask = *(u16 *)(*(u32 *)(ctx + 4) + 0x86);
        newSlot = (dsInfo->doubleMode == 1) ? (u16)((next + 1) % 4) : next;

        zero = 0;
        MultiStore16(zero, &dsInfo->ds[newSlot], 0x100);

        dsInfo->ds[newSlot].aidHeader = (u16)(dsInfo->aidBitmap & (mask | 1));
        dsInfo->writeIndex = next;
        dsInfo->ds[prevSlot].aidHeader = dsInfo->aidBitmap;
        if (excludeParent == 1) {
            dsInfo->ds[prevSlot].aidHeader &= ~1;
        }

        IRQ::Restore(saved);

        ret = WM_SetMPDataToPort(WmDataSharingSetDataCallback, (int)&dsInfo->ds[prevSlot],
                                 dsInfo->dataSetLength, (u16)(dsInfo->aidBitmap & mask),
                                 dsInfo->port, 1);

        if (ret == 7) {
            dsInfo->seqNum[prevSlot] = 0xffff;
            dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % 4);
        } else if (ret != 0 && ret != 2) {
            dsInfo->state = 3;
        }
    } else {
        IRQ::Restore(saved);
    }
}

/* NitroSDK WM_GetSharedDataAddress: where member aid's data sits in a
 * received data set, or 0 for a null dsInfo or receiveData or when the set
 * holds nothing from aid (its receivedBitmap bit is clear). */
// @symbol WM_GetSharedDataAddress
unsigned WM_GetSharedDataAddress(char *dsInfo, WMDataSet *receiveData, int aid) {
    if (dsInfo == 0)
        return 0;
    if (receiveData == 0)
        return 0;
    if ((receiveData->receivedBitmap & (1u << aid)) == 0)
        return 0;
    return (unsigned)WmGetSharedDataAddress((WMDataSharingInfo *)dsInfo, receiveData->aidHeader,
                                            receiveData->data, aid);
}

/* Member aid's slot in a set: receiveBuf, past dataLength bytes for each
 * member below aid in aidBitmap. pret's later SDK counts them with
 * MATH_CountPopulation; this one steps the bits itself. */
// @symbol WmGetSharedDataAddress
u16 *WmGetSharedDataAddress(WMDataSharingInfo *dsInfo, unsigned aidBitmap, u16 *receiveBuf,
                            int aid) {
    int i;
    u16 bit = 1;
    for (i = 0; i < aid; i++) {
        if (aidBitmap & bit)
            receiveBuf = (u16 *)((u8 *)receiveBuf + dsInfo->dataLength);
        bit <<= 1;
    }
    return receiveBuf;
}
}
