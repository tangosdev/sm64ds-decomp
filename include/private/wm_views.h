#ifndef PRIVATE_WM_VIEWS_H
#define PRIVATE_WM_VIEWS_H

#include "WmWork.h"

/* NitroSDK WMArm9Buf, the part of it the WM_* wrappers read through
 * WM_GetSystemWork(); WMStatus, the status block it points at, is public
 * and lives in nitro/wm/wm.h. Plain C types: some consumers do not include
 * types.h.
 *
 * WM_Init carves the 0x1300-byte WM system buffer wm9buf into ARM7's
 * work area (WM7), the status block, the two FIFO blocks and these callback
 * tables. callbackTable has one slot per WM API id below 0x25
 * (WM_NUM_OF_CALLBACK); WmReceiveFifo
 * handles ids from 0x25 up (the indication, port and 0x81 events) apart. */
typedef struct WMArm9Buf {
    void *WM7;        /* 0x000 -- buf + 0x600, ARM7's work area */
    WMStatus *status; /* 0x004 */
    char pad_08[0xc - 0x8];
    void *fifo9to7;                 /* 0x00c -- command block sent to ARM7, 0x100 bytes */
    void *fifo7to9;                 /* 0x010 -- callback block ARM7 answers in, 0x100 bytes */
    WMCallbackFunc callbackTable[WM_NUM_OF_CALLBACK]; /* 0x014 -- by API id */
    WMCallbackFunc indCallback;     /* 0x0a8 -- the indication callback (op 0x80) */
    WMCallbackFunc portCallbackTable[WM_NUM_OF_PORT]; /* 0x0ac */
    void *portCallbackArgument[WM_NUM_OF_PORT];       /* 0x0ec */
    char pad_12c[0x13c - 0x12c];
    int initCleared; /* 0x13c -- WM_Init zeroes it; nothing else in src/ touches it */
    char pad_140[0x560 - 0x140];
    unsigned short dmaNo; /* 0x560 -- the DMA channel WM_Init was given */
} WMArm9Buf;

/* Leading fields of NitroSDK WMStartParentCallback, the argument the WM
 * parent callback (MBi_ParentCallback) passes for CONNECTED (code 0),
 * DISCONNECTED (code 1) and BEACON_SENT (code 0x1c). Codes 0xff (a failed
 * WM call, from MBi_CheckWmErrcode: {apiid, errcode}) and 0x100 (any other WM
 * result) read only the WMCallback header the SDK callbacks all share. */
typedef struct WmStartParentCallback {
    unsigned short apiid;    /* 0x00 */
    unsigned short errcode;  /* 0x02 */
    unsigned short wlCmdID;  /* 0x04 */
    unsigned short wlResult; /* 0x06 */
    /* 0x08 -- 0 PARENT_START, 2 BEACON_SENT, 7 CONNECTED, 9 DISCONNECTED */
    unsigned short state;
    unsigned char macAddress[6]; /* 0x0a -- the child's */
    unsigned short aid;          /* 0x10 */
} WmStartParentCallback;

/* NitroSDK WMPortRecvCallback. The offsets agree with every port receive
 * callback that reads one (WM_Ds.c's two data-sharing receive callbacks
 * and the file-sharing one, WFSi_PortCallback). Nothing here reads past arg, but
 * the size is the SDK's: WmReceiveFifo's static cb_Port starts at 0x020a89ec
 * and the 32-aligned fifoBuf after it at 0x020a8a40, which leaves it
 * 0x35..0x54 bytes, and pret's WM_system.s keeps the same gap. */
typedef struct WMPortRecvCallback {
    unsigned short apiid;              /* 0x00 */
    unsigned short errcode;            /* 0x02 */
    unsigned short state;              /* 0x04 -- 7 CONNECTED, 9 DISCONNECTED, 0x15 PORT_RECV */
    unsigned short port;               /* 0x06 */
    void *recvBuf;                     /* 0x08 */
    unsigned short *data;              /* 0x0c */
    unsigned short length;             /* 0x10 -- bytes at data */
    unsigned short aid;                /* 0x12 -- the sender */
    unsigned char macAddress[6];       /* 0x14 */
    unsigned short seqNo;              /* 0x1a */
    void *arg;                         /* 0x1c -- WM_SetPortCallback's argument */
    unsigned short myAid;              /* 0x20 */
    unsigned short connectedAidBitmap; /* 0x22 */
    unsigned char ssid[24];            /* 0x24 -- WM_SIZE_CHILD_SSID */
    unsigned short reason;             /* 0x3c */
    unsigned short rsv;                /* 0x3e */
    unsigned short maxSendDataSize;    /* 0x40 */
    unsigned short maxRecvDataSize;    /* 0x42 */
} WMPortRecvCallback;

/* NitroSDK WMDataSet, one data-sharing frame (0x100 bytes): a 4-byte header,
 * then each member's dataLength bytes in AID order. */
typedef struct WMDataSet {
    unsigned short aidHeader;      /* 0x00 -- members whose data the set holds */
    unsigned short receivedBitmap; /* 0x02 -- members whose data arrived */
    unsigned short data[WM_DS_DATA_SIZE / sizeof(unsigned short)]; /* 0x04 */
} WMDataSet;

typedef char WMDataSet_size_must_be_0x100[sizeof(WMDataSet) == 0x100 ? 1 : -1];

/* NitroSDK WMDataSharingInfo (0x420; WM_StartDataSharing clears that many
 * bytes): four data sets in a ring, then the ring indices. writeIndex is the
 * set being filled, sendIndex the next to send and readIndex the next
 * WM_StepDataSharing hands over. */
typedef struct WMDataSharingInfo {
    WMDataSet ds[WM_DS_DATASET_NUM];          /* 0x000 */
    unsigned short seqNum[WM_DS_DATASET_NUM]; /* 0x400 -- each set's MP sequence number */
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
    unsigned short state;         /* 0x41c -- WMDataSharingState */
    unsigned short reserved[1];   /* 0x41e */
} WMDataSharingInfo;

typedef char WMDataSharingInfo_size_must_be_0x420[sizeof(WMDataSharingInfo) == 0x420 ? 1 : -1];

/* The SDK-private calls every WM_* wrapper makes; NitroSDK spells them
 * WMi_GetSystemWork, WMi_CheckStateEx, WMi_SendCommand and
 * WMi_SetCallbackTable. The check and the send are variadic: paramNum
 * states to accept, or paramNum words to copy into the request block.
 * WM_SetCallbackTable stores a request's callback in callbackTable by API
 * id. */
#ifdef __cplusplus
extern "C"
#endif
#ifdef __cplusplus
#ifdef _MSC_VER
    extern "C" {
#endif
#endif
    WMArm9Buf *WM_GetSystemWork(void);
#ifdef __cplusplus
    extern "C"
#endif
        WMErrCode
        WM_CheckStateEx(int paramNum, ...);
#ifdef __cplusplus
    extern "C"
#endif
        WMErrCode
        WM_SendCommand(WMApiid id, unsigned short paramNum, ...);
#ifdef __cplusplus
    extern "C"
#endif
        void
        WM_SetCallbackTable(WMApiid id, WMCallbackFunc callback);
#ifdef __cplusplus
    extern "C"
#endif
        WMErrCode
        WMi_CheckIdle(void);
#ifdef __cplusplus
    extern "C"
#endif
        WMErrCode
        WMi_CheckInitialized(void);

#ifdef __cplusplus
    extern "C"
#endif
        /* NitroSDK wm_ds.c (WM_Ds.c): data sharing. This SDK build takes the
 * bitmap, length and mode as ints. */
        WMErrCode
        WM_StartDataSharing(WMDataSharingInfo *, unsigned short, int, int, int);
#ifdef __cplusplus
    extern "C"
#endif
        WMErrCode
        WM_StepDataSharing(WMDataSharingInfo *, const void *, WMDataSet *);
#ifdef __cplusplus
    extern "C"
#endif
        unsigned short *
        WM_GetSharedDataAddress(WMDataSharingInfo *, WMDataSet *, int);
#ifdef __cplusplus
#ifdef _MSC_VER
}
#endif
#endif

#endif
