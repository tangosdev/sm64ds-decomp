//cpp
#include "types.h"

namespace CP15 {
void InvalidateDataCache(u32 addr, u32 sz);
}
extern "C" int WM_GetSystemWork(void);

extern "C" void MultiCopyHalf(const void *src, void *dst, u32 count);

/* NitroSDK data sharing, the child's port receive callback: WM_StartDataSharing
 * (func_020631dc) registers it with WM_SetPortCallback (func_02061c88) when this
 * station is not the parent (status->aid != 0). On PORT_RECV (state 0x15) it
 * copies the parent's whole data set into ds[writeIndex], records its sequence
 * number and steps writeIndex; CONNECTED (7) and DISCONNECTED (9) are ignored.
 * A failed receive (errcode != 0) puts the sharing into state 3, the error
 * state WM_StepDataSharing (func_02062df0) reports. */

/* NitroSDK WMPortRecvCallback, the leading fields: the offsets agree with every
 * port receive callback in this cluster (func_02062bdc, func_020417d0). */
struct WMPortRecvCallback {
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
};

/* NitroSDK WMDataSet / WMDataSharingInfo (0x420; WM_StartDataSharing clears
 * that many bytes): four 0x100-byte data sets, each a 4-byte header and every
 * member's dataLength bytes in AID order, then the ring indices. */
struct WMDataSet {
    u16 aidHeader;      /* 0x00 -- members whose data the set holds */
    u16 receivedBitmap; /* 0x02 */
    u16 data[0x7e];     /* 0x04 */
};

struct WMDataSharingInfo {
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
};

/* WM_GetSystemWork's WMArm9Buf and its WMStatus, as func_02061ab0 views them. */
struct WMStatus {
    u8 pad_000[0x184];
    u16 aid; /* 0x184 -- 0 for the parent */
};

struct WMArm9Buf {
    u8 pad_00[4];
    WMStatus *status; /* 0x04 */
};

extern "C" void func_02062aa4(char *c)
{
    WMArm9Buf *h = (WMArm9Buf *)WM_GetSystemWork();
    WMPortRecvCallback *cb = (WMPortRecvCallback *)c;
    WMDataSharingInfo *r5 = (WMDataSharingInfo *)cb->arg;
    if (cb->errcode == 0) {
        u32 r4;
        u16 *src;
        {
            u16 v = cb->state;
            if (v == 7) return;
            if (v == 9) return;
            if (v != 0x15) return;
        }
        r4 = cb->length;
        if (r4 != r5->dataSetLength) {
            if (r4 > 0x100)
                r4 = 0x100;
        }
        CP15::InvalidateDataCache((u32)&h->status->aid, 2);
        if (r4 < 4)
            return;
        src = cb->data;
        if (*src == 0)
            return;
        if ((1u << h->status->aid) == 0)
            return;
        MultiCopyHalf(src, &r5->ds[r5->writeIndex], r4);
        r5->seqNum[r5->writeIndex] = cb->seqNo >> 1;
        r5->writeIndex = (r5->writeIndex + 1) % 4;
    } else {
        r5->state = 3;
    }
}
