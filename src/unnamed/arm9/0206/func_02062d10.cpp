//cpp
#include "types.h"
extern "C" void *WM_GetSystemWork(void);

struct CP15 { static void InvalidateDataCache(u32 addr, u32 len); };

/* NitroSDK data sharing, the port send callback: WM_StartDataSharing
 * (func_020631dc) and the send step (func_020627e8, func_02062df0) pass it to
 * the port send (func_020623ec). The sharing it belongs to is the port's
 * WM_SetPortCallback argument. On success the parent records the sent set's
 * sequence number and steps sendIndex; a full send queue (errcode 0xa) backs
 * the parent's sendIndex up and asks for a retry (state 2); any other failure
 * is the error state 3. */

/* NitroSDK WMPortSendCallback: only the fields read here. */
struct WMPortSendCallback {
    char pad0[2];
    u16 errcode; /* 0x02 -- 0 sent, 0xa the send queue was full */
    char pad1[0xa - 0x4];
    u16 port; /* 0x0a */
    char pad2[0x1c - 0xc];
    u16 seqNo; /* 0x1c */
};

/* NitroSDK WMDataSet / WMDataSharingInfo, as func_02062aa4 spells them. */
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

/* WM_GetSystemWork's WMArm9Buf and its WMStatus, as func_02061ab0 views them;
 * WM_SetPortCallback (func_02061c88) fills the per-port tables. */
struct WMStatus {
    u8 pad_000[0x184];
    u16 aid; /* 0x184 -- 0 for the parent */
};

struct WMArm9Buf {
    u8 pad_00[4];
    WMStatus *status; /* 0x04 */
    u8 pad_08[0xac - 0x8];
    void *portCallbackTable[16];    /* 0x0ac */
    void *portCallbackArgument[16]; /* 0x0ec */
};

extern "C" void func_02062d10(WMPortSendCallback *self) {
    WMArm9Buf *c = (WMArm9Buf *)WM_GetSystemWork();
    WMDataSharingInfo *r4 = (WMDataSharingInfo *)c->portCallbackArgument[self->port];
    CP15::InvalidateDataCache((u32)&c->status->aid, 2);
    u16 v = c->status->aid;
    int kind = self->errcode;
    if (kind == 0) {
        if (v != 0) return;
        r4->seqNum[r4->sendIndex] = (u16)(self->seqNo >> 1);
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
