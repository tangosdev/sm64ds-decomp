/* NitroSDK wm_ds.c: WM data sharing. Every MP frame the parent gathers one
 * data set (a 4-byte header and every member's dataLength bytes in AID
 * order) and sends it to the children, who send their own data back.
 * dsInfo holds four sets in a ring; writeIndex is the set being filled,
 * sendIndex the next to send and readIndex the next WM_StepDataSharing
 * hands over. This SDK is older than pret's pokediamond: the sets go out
 * with WM_SetMPDataToPort, WM_DS_STATE_ERROR is 3, not 5, and
 * WM_EndDataSharing is not linked. */
#include "types.h"
#include "nitro/mi/memory.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/wm/wm_api.h"
#include "private/wm_views.h"

/* NitroSDK WMPortSendCallback: only the fields read here. */
typedef struct WMPortSendCallback {
    char pad0[2];
    u16 errcode; /* 0x02 */
    char pad1[0xa - 0x4];
    u16 port; /* 0x0a */
    char pad2[0x1c - 0xc];
    u16 seqNo; /* 0x1c */
} WMPortSendCallback;


void WmDataSharingSetDataCallback(void *arg);
void WmDataSharingReceiveCallback_Parent(void *arg);
void WmDataSharingReceiveCallback_Child(void *arg);
void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, int aid, const void *data);
void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, int excludeParent);
u16 *WmGetSharedDataAddress(WMDataSharingInfo *dsInfo, unsigned aidBitmap, u16 *receiveBuf,
                            int aid);

/* NitroSDK WM_StartDataSharing(dsInfo, port, aidBitmap, dataLength,
 * doubleMode): with MP running, clear the sharing info, add this station's
 * own AID bit to aidBitmap and size a data set as dataLength per member
 * plus the WM_DS_HEADER_SIZE header (at most WM_DS_DATA_SIZE before it). The parent
 * (aid 0) primes the four sets, registers its port receive callback and
 * sends the first set (two in double mode); a child registers its own
 * receive callback and waits. */
WMErrCode WM_StartDataSharing(WMDataSharingInfo *dsInfo, u16 port, int aidBitmap, int dataLength,
                              int doubleMode) {
    u16 childMask = 1;
    WMArm9Buf *sys;
    WMErrCode r;
    u16 aid;
    u16 cnt;
    u16 j;
    u16 mbit;
    u16 bits;
    volatile unsigned zero;

    sys = WM_GetSystemWork();
    r = WM_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    if (r != WM_ERRCODE_SUCCESS)
        return r;

    DC_InvalidateRange(&sys->status->mp_flag, 4);
    if (sys->status->mp_flag == 0)
        return WM_ERRCODE_ILLEGAL_STATE;

    if (dsInfo == 0)
        return WM_ERRCODE_INVALID_PARAM;
    if (port >= WM_NUM_OF_PORT)
        return WM_ERRCODE_INVALID_PARAM;
    if (aidBitmap == 0)
        return WM_ERRCODE_INVALID_PARAM;

    DC_InvalidateRange(&sys->status->aid, 2);
    aid = sys->status->aid;
    if (aid == 0) {
        DC_InvalidateRange(&sys->status->mp_readyBitmap, 2);
        childMask = sys->status->mp_readyBitmap;
    }

    zero = 0;
    MI_CpuFillFast(dsInfo, zero, sizeof(WMDataSharingInfo));

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
    if (dsInfo->dataSetLength > WM_DS_DATA_SIZE) {
        dsInfo->aidBitmap = 0;
        return WM_ERRCODE_INVALID_PARAM;
    }

    dsInfo->dataSetLength += WM_DS_HEADER_SIZE;
    dsInfo->state = WM_DS_STATE_START;

    if (aid == 0) {
        int i;
        WMDataSet *cur;
        int n;

        for (i = 0; i < WM_DS_DATASET_NUM; i++)
            dsInfo->ds[i].aidHeader = (u16)(dsInfo->aidBitmap & (childMask | 1));

        WM_SetPortCallback(port, WmDataSharingReceiveCallback_Parent, dsInfo);

        cur = dsInfo->ds;
        for (n = 0; n < ((dsInfo->doubleMode == 1) ? 2 : 1); n++) {
            WMErrCode cr;

            dsInfo->writeIndex = (dsInfo->writeIndex + 1) % WM_DS_DATASET_NUM;
            cr = WM_SetMPDataToPort(WmDataSharingSetDataCallback, (u16 *)cur, dsInfo->dataSetLength,
                                    (u16)(dsInfo->aidBitmap & childMask), dsInfo->port,
                                    WM_PRIORITY_HIGH);

            if (cr == WM_ERRCODE_NO_CHILD) {
                dsInfo->seqNum[n] = 0xffff;
                dsInfo->sendIndex = (dsInfo->sendIndex + 1) % WM_DS_DATASET_NUM;
            } else if (cr != WM_ERRCODE_SUCCESS && cr != WM_ERRCODE_OPERATING) {
                dsInfo->state = WM_DS_STATE_ERROR;
                return WM_ERRCODE_FAILED;
            }

            cur++;
        }
    } else {
        dsInfo->sendIndex = 3;
        WM_SetPortCallback(port, WmDataSharingReceiveCallback_Child, dsInfo);
    }

    return WM_ERRCODE_SUCCESS;
}

/* NitroSDK WM_StepDataSharing(dsInfo, sendData, receiveData): one frame of
 * data sharing. The parent (aid 0) resends a set the last send refused,
 * hands the oldest complete set to receiveData, then merges its own
 * sendData into the set being filled (WmDataSharingReceiveData) and sends
 * whatever is ready (WmDataSharingSendDataSet). A child arms or hands over
 * the received set and sends its own data to the parent. Returns 0 with a
 * set handed over, NO_DATASET when none was ready, FAILED on a send error. */
WMErrCode WM_StepDataSharing(WMDataSharingInfo *dsInfo, const void *sendData,
                             WMDataSet *receiveData) {
    WMErrCode result;
    u16 aid;
    u16 childMask;
    int did;
    int fp;
    int state;
    WMArm9Buf *p = WM_GetSystemWork();

    result = WM_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
        return result;

    DC_InvalidateRange(&p->status->mp_flag, 4);
    if (p->status->mp_flag == 0)
        return WM_ERRCODE_ILLEGAL_STATE;

    if (dsInfo == 0)
        return WM_ERRCODE_INVALID_PARAM;
    if (sendData == 0)
        return WM_ERRCODE_INVALID_PARAM;
    if (receiveData == 0)
        return WM_ERRCODE_INVALID_PARAM;

    DC_InvalidateRange(&p->status->aid, 2);
    aid = p->status->aid;
    if (aid == 0) {
        DC_InvalidateRange(&p->status->mp_readyBitmap, 2);
        childMask = p->status->mp_readyBitmap;
    }

    state = dsInfo->state;
    if (state == WM_DS_STATE_ERROR)
        return WM_ERRCODE_FAILED;
    if (state != WM_DS_STATE_START && state != WM_DS_STATE_RETRY_SEND)
        return WM_ERRCODE_ILLEGAL_STATE;

    result = WM_ERRCODE_NO_DATASET;

    if (aid == 0) {
        did = 0;
        fp = did;

        if (state == WM_DS_STATE_RETRY_SEND) {
            u16 n;
            WMErrCode cr;
            dsInfo->state = WM_DS_STATE_START;
            n = (u16)((dsInfo->writeIndex + WM_DS_DATASET_NUM - 1) % WM_DS_DATASET_NUM);
            cr = WM_SetMPDataToPort(WmDataSharingSetDataCallback, &dsInfo->ds[n].aidHeader,
                                    dsInfo->dataSetLength, (u16)(dsInfo->aidBitmap & childMask),
                                    dsInfo->port, WM_PRIORITY_HIGH);
            if (cr == WM_ERRCODE_NO_CHILD) {
                dsInfo->seqNum[n] = 0xffff;
                dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % WM_DS_DATASET_NUM);
            } else if (cr != WM_ERRCODE_SUCCESS && cr != WM_ERRCODE_OPERATING) {
                dsInfo->state = WM_DS_STATE_ERROR;
                return WM_ERRCODE_FAILED;
            }
        }

        if (dsInfo->readIndex != dsInfo->sendIndex) {
            dsInfo->ds[dsInfo->readIndex].aidHeader |= 1;
            MI_CpuCopy16(&dsInfo->ds[dsInfo->readIndex], receiveData, sizeof(WMDataSet));
            did = 1;
            result = WM_ERRCODE_SUCCESS;
            dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
            dsInfo->readIndex = (u16)((dsInfo->readIndex + 1) % WM_DS_DATASET_NUM);

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
        if (state == WM_DS_STATE_RETRY_SEND) {
            dsInfo->state = WM_DS_STATE_START;
            changed = 1;
        } else {
            if (dsInfo->readIndex != dsInfo->writeIndex) {
                if ((dsInfo->ds[dsInfo->readIndex].aidHeader & 1) == 0) {
                    dsInfo->ds[dsInfo->readIndex].aidHeader |= 1;
                } else {
                    MI_CpuCopy16(&dsInfo->ds[dsInfo->readIndex], receiveData, sizeof(WMDataSet));
                    changed = 1;
                    result = WM_ERRCODE_SUCCESS;
                    dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
                    dsInfo->readIndex = (u16)((dsInfo->readIndex + 1) % WM_DS_DATASET_NUM);
                }
            }
        }

        if (changed != 0) {
            /* the child stages its own data 0x20 bytes into the set it sends */
            u16 *addr = &dsInfo->ds[dsInfo->sendIndex].data[14];
            WMErrCode cr;
            MI_CpuCopy16(sendData, addr, dsInfo->dataLength);
            cr = WM_SetMPDataToPort(WmDataSharingSetDataCallback, addr, dsInfo->dataLength,
                                    dsInfo->aidBitmap, dsInfo->port, WM_PRIORITY_HIGH);
            dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % WM_DS_DATASET_NUM);
            if (cr != WM_ERRCODE_OPERATING && cr != WM_ERRCODE_SUCCESS) {
                dsInfo->state = WM_DS_STATE_ERROR;
                result = WM_ERRCODE_FAILED;
            }
        }
    }

    return result;
}

/* The port send callback WM_StartDataSharing, WM_StepDataSharing and
 * WmDataSharingSendDataSet pass to WM_SetMPDataToPort. The sharing it
 * belongs to is the port's WM_SetPortCallback argument. On success the
 * parent records the sent set's sequence number and steps sendIndex; a full
 * send queue backs a child's sendIndex up and asks for a retry
 * (WM_DS_STATE_RETRY_SEND); any other failure is WM_DS_STATE_ERROR. */
void WmDataSharingSetDataCallback(void *arg) {
    WMPortSendCallback *callback = (WMPortSendCallback *)arg;
    WMArm9Buf *c = WM_GetSystemWork();
    WMDataSharingInfo *r4 = (WMDataSharingInfo *)c->portCallbackArgument[callback->port];
    DC_InvalidateRange(&c->status->aid, 2);
    u16 v = c->status->aid;
    int kind = callback->errcode;
    if (kind == WM_ERRCODE_SUCCESS) {
        if (v != 0)
            return;
        r4->seqNum[r4->sendIndex] = (u16)(callback->seqNo >> 1);
        r4->sendIndex = (u16)((r4->sendIndex + 1) % WM_DS_DATASET_NUM);
    } else if (kind == WM_ERRCODE_SEND_QUEUE_FULL) {
        if (v != 0) {
            r4->sendIndex = (u16)((r4->sendIndex + WM_DS_DATASET_NUM - 1) % WM_DS_DATASET_NUM);
        }
        r4->state = WM_DS_STATE_RETRY_SEND;
    } else {
        r4->state = WM_DS_STATE_ERROR;
    }
}

/* The parent's port receive callback. PORT_RECV stores the child's data
 * and tries to send the next set; CONNECTED just tries the send;
 * DISCONNECTED stops waiting for that child in the current set (and the
 * next one in double mode) before sending. A failed receive puts the
 * sharing into WM_DS_STATE_ERROR. */
void WmDataSharingReceiveCallback_Parent(void *arg) {
    WMPortRecvCallback *callback = (WMPortRecvCallback *)arg;
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)callback->arg;
    if (callback->errcode == WM_ERRCODE_SUCCESS) {
        switch (callback->state) {
        case WM_STATECODE_PORT_RECV:
            WmDataSharingReceiveData(dsInfo, callback->aid, callback->data);
            WmDataSharingSendDataSet(dsInfo, 0);
            return;
        case WM_STATECODE_CONNECTED:
            WmDataSharingSendDataSet(dsInfo, 0);
            return;
        case WM_STATECODE_DISCONNECTED: {
            u16 mask = (u16)(1 << callback->aid);
            OSIntrMode saved = OS_DisableInterrupts();
            u16 b408 = dsInfo->writeIndex;
            dsInfo->ds[b408].aidHeader &= ~mask;
            if (dsInfo->doubleMode == 1) {
                int n = (dsInfo->writeIndex + 1) % WM_DS_DATASET_NUM;
                dsInfo->ds[n].aidHeader &= ~mask;
            }
            OS_RestoreInterrupts(saved);
            WmDataSharingSendDataSet(dsInfo, 0);
            if (dsInfo->doubleMode == 1)
                WmDataSharingSendDataSet(dsInfo, 0);
            return;
        }
        default:
            return;
        }
    }
    dsInfo->state = WM_DS_STATE_ERROR;
}

/* The child's port receive callback. On PORT_RECV it copies the parent's
 * whole data set into ds[writeIndex], records its sequence number and steps
 * writeIndex; CONNECTED and DISCONNECTED are ignored. A failed receive puts
 * the sharing into WM_DS_STATE_ERROR. */
void WmDataSharingReceiveCallback_Child(void *arg) {
    WMPortRecvCallback *callback = (WMPortRecvCallback *)arg;
    WMArm9Buf *h = WM_GetSystemWork();
    WMDataSharingInfo *r5 = (WMDataSharingInfo *)callback->arg;
    if (callback->errcode == WM_ERRCODE_SUCCESS) {
        u32 r4;
        u16 *src;
        {
            u16 v = callback->state;
            if (v == WM_STATECODE_CONNECTED)
                return;
            if (v == WM_STATECODE_DISCONNECTED)
                return;
            if (v != WM_STATECODE_PORT_RECV)
                return;
        }
        r4 = callback->length;
        if (r4 != r5->dataSetLength) {
            if (r4 > sizeof(WMDataSet))
                r4 = sizeof(WMDataSet);
        }
        DC_InvalidateRange(&h->status->aid, 2);
        if (r4 < WM_DS_HEADER_SIZE)
            return;
        src = callback->data;
        if (*src == 0)
            return;
        if ((1u << h->status->aid) == 0)
            return;
        MI_CpuCopy16(src, &r5->ds[r5->writeIndex], r4);
        r5->seqNum[r5->writeIndex] = callback->seqNo >> 1;
        r5->writeIndex = (r5->writeIndex + 1) % WM_DS_DATASET_NUM;
    } else {
        r5->state = WM_DS_STATE_ERROR;
    }
}

/* Store the data member aid sent into the data set still waiting for it
 * (ds[writeIndex], or in double mode the next one), at that member's slot,
 * or zero the slot when there is no data. The bit then moves from the set's
 * aidHeader to its receivedBitmap. */
void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, int aid, const void *data) {
    u16 mask = (u16)(1 << aid);
    u16 aidBitmap = dsInfo->aidBitmap;
    u16 idx;
    volatile u16 zero;
    u16 *slot;
    OSIntrMode s;

    if ((aidBitmap & mask) == 0)
        return;

    idx = dsInfo->writeIndex;
    if ((mask & dsInfo->ds[idx].aidHeader) == 0) {
        if (dsInfo->doubleMode != 1)
            return;
        idx = (idx + 1) % WM_DS_DATASET_NUM;
        if ((mask & dsInfo->ds[idx].aidHeader) == 0)
            return;
    }

    slot = WmGetSharedDataAddress(dsInfo, aidBitmap, dsInfo->ds[idx].data, aid);
    if (data != 0) {
        MI_CpuCopy16(data, slot, dsInfo->dataLength);
    } else {
        zero = 0;
        MI_CpuFill16(slot, zero, dsInfo->dataLength);
    }
    s = OS_DisableInterrupts();
    dsInfo->ds[idx].aidHeader &= ~mask;
    dsInfo->ds[idx].receivedBitmap |= mask;
    OS_RestoreInterrupts(s);
}

/* Once every member has filled ds[writeIndex], step writeIndex, open the
 * next set (the one after it in double mode) for the ready children and the
 * parent, and send the full set to the children. excludeParent clears the
 * parent's own bit from the sent header. With no child to send to
 * (NO_CHILD) the set counts as sent; any failure but OPERATING is
 * WM_DS_STATE_ERROR. */
void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, int excludeParent) {
    WMArm9Buf *ctx;
    OSIntrMode saved;
    u16 slot;
    u16 next;
    u16 mask;
    u16 prevSlot;
    u16 newSlot;
    volatile u16 zero;
    WMErrCode ret;

    ctx = WM_GetSystemWork();
    saved = OS_DisableInterrupts();
    slot = dsInfo->writeIndex;
    if (dsInfo->ds[slot].aidHeader == 0) {
        DC_InvalidateRange(&ctx->status->mp_readyBitmap, 2);

        prevSlot = dsInfo->writeIndex;
        next = (u16)((prevSlot + 1) % WM_DS_DATASET_NUM);
        mask = ctx->status->mp_readyBitmap;
        newSlot = (dsInfo->doubleMode == 1) ? (u16)((next + 1) % WM_DS_DATASET_NUM) : next;

        zero = 0;
        MI_CpuFill16(&dsInfo->ds[newSlot], zero, sizeof(WMDataSet));

        dsInfo->ds[newSlot].aidHeader = (u16)(dsInfo->aidBitmap & (mask | 1));
        dsInfo->writeIndex = next;
        dsInfo->ds[prevSlot].aidHeader = dsInfo->aidBitmap;
        if (excludeParent == 1) {
            dsInfo->ds[prevSlot].aidHeader &= ~1;
        }

        OS_RestoreInterrupts(saved);

        ret = WM_SetMPDataToPort(WmDataSharingSetDataCallback, &dsInfo->ds[prevSlot].aidHeader,
                                 dsInfo->dataSetLength, (u16)(dsInfo->aidBitmap & mask),
                                 dsInfo->port, WM_PRIORITY_HIGH);

        if (ret == WM_ERRCODE_NO_CHILD) {
            dsInfo->seqNum[prevSlot] = 0xffff;
            dsInfo->sendIndex = (u16)((dsInfo->sendIndex + 1) % WM_DS_DATASET_NUM);
        } else if (ret != WM_ERRCODE_SUCCESS && ret != WM_ERRCODE_OPERATING) {
            dsInfo->state = WM_DS_STATE_ERROR;
        }
    } else {
        OS_RestoreInterrupts(saved);
    }
}

/* NitroSDK WM_GetSharedDataAddress: where member aid's data sits in a
 * received data set, or 0 for a null dsInfo or receiveData or when the set
 * holds nothing from aid (its receivedBitmap bit is clear). */
u16 *WM_GetSharedDataAddress(WMDataSharingInfo *dsInfo, WMDataSet *receiveData, int aid) {
    if (dsInfo == 0)
        return 0;
    if (receiveData == 0)
        return 0;
    if ((receiveData->receivedBitmap & (1u << aid)) == 0)
        return 0;
    return WmGetSharedDataAddress(dsInfo, receiveData->aidHeader, receiveData->data, aid);
}

/* Member aid's slot in a set: receiveBuf, past dataLength bytes for each
 * member below aid in aidBitmap. pret's later SDK counts them with
 * MATH_CountPopulation; this one steps the bits itself. */
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
