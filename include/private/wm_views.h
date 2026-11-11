#ifndef PRIVATE_WM_VIEWS_H
#define PRIVATE_WM_VIEWS_H

#include "WmWork.h"

/* NitroSDK WMArm9Buf / WMStatus, the part of them the WM_* wrappers read
 * through WM_GetSystemWork(). One definition for every wrapper in src/ that
 * used to carry its own padded copy. Each offset below is one that some
 * wrapper reads at that width; everything else stays padding.
 *
 * Offsets are this ROM's SDK revision, not the public NitroSDK header's:
 * linkLevel and child_bitmap sit 4 bytes lower than in pret's pokediamond.
 * Plain C types: some consumers do not include types.h. */
typedef struct WMStatus {
    unsigned short state; /* 0x000 -- 7/8 idle, 9 MP_PARENT, 0xa MP_CHILD, 0xb DCF_CHILD */
    char pad_002[0x10 - 0x2];
    int mp_flag; /* 0x010 -- 1 once MP has started */
    char pad_014[0x46 - 0x14];
    unsigned short
        mp_recvBufSize; /* 0x046 -- size WmReceiveFifo invalidates at a callback's recvBuf */
    char pad_048[0x86 - 0x48];
    unsigned short mp_readyBitmap; /* 0x086 -- children the parent's data sets wait for */
    char pad_088[0xb8 - 0x88];
    unsigned short linkLevel; /* 0x0b8 -- WMLinkLevel, 0 (weakest) to 3 */
    char pad_0ba[0xe4 - 0xba];
    WmParentParam pparam; /* 0x0e4 -- maxEntry (+0xe) read by WM_GetMPReceiveBufferSize */
    char pad_124[0x17e - 0x124];
    unsigned short child_bitmap; /* 0x17e -- connected children, by AID bit */
    char pad_180[0x184 - 0x180];
    unsigned short aid; /* 0x184 -- 0 for the parent */
    char pad_186[0x18e - 0x186];
    unsigned short mp_maxSendSize; /* 0x18e */
    unsigned short mp_maxRecvSize; /* 0x190 */
    char pad_192[0x1f2 - 0x192];
    unsigned short allowedChannel; /* 0x1f2 -- bit n-1 set when channel n is usable */
} WMStatus;

typedef char WMStatus_size_must_be_0x1f4[sizeof(WMStatus) == 0x1f4 ? 1 : -1];

/* WM_Init carves the 0x1300-byte WM system buffer data_020a89ac into ARM7's
 * work area (WM7), the status block, the two FIFO blocks and these callback
 * tables. callbackTable has one slot per WM API id below 0x25; WmReceiveFifo
 * handles ids from 0x25 up (the indication, port and 0x81 events) apart. */
typedef struct WMArm9Buf {
    void *WM7;        /* 0x000 -- buf + 0x600, ARM7's work area */
    WMStatus *status; /* 0x004 */
    char pad_08[0xc - 0x8];
    void *fifo9to7;                 /* 0x00c -- command block sent to ARM7, 0x100 bytes */
    void *fifo7to9;                 /* 0x010 -- callback block ARM7 answers in, 0x100 bytes */
    void *callbackTable[0x25];      /* 0x014 -- by API id */
    void *indCallback;              /* 0x0a8 -- the indication callback (op 0x80) */
    void *portCallbackTable[16];    /* 0x0ac */
    void *portCallbackArgument[16]; /* 0x0ec */
    char pad_12c[0x13c - 0x12c];
    int initCleared; /* 0x13c -- WM_Init zeroes it; nothing else in src/ touches it */
    char pad_140[0x560 - 0x140];
    unsigned short dmaNo; /* 0x560 -- the DMA channel WM_Init was given */
} WMArm9Buf;

/* NitroSDK WMBssDesc, the beacon record of a parent (0xc0 bytes: the SDK's
 * WM_SIZE_BSSDESC). Names are the SDK's; this SDK build reads and writes the
 * ssid and ggid as halfwords, so ssid is u16[16] where the public header has
 * u8[32] and gameInfo.ggid is u16[2]; the types choose the load and store
 * widths, so they stay as the code uses them. Only offsets the ROM's code
 * reads are named. */
typedef struct WMBssDesc {
    unsigned char pad_00[0xc];
    unsigned short ssid[0x10]; /* 0x0c -- u8[32] in the SDK */
    unsigned char pad_2c[0xa];
    unsigned short channel; /* 0x36 */
    unsigned char pad_38[4];
    unsigned short gameInfoLength; /* 0x3c */
    unsigned char pad_3e[6];
    struct {
        unsigned short ggid[2];           /* 0x44 */
        unsigned short tgid;              /* 0x48 */
        unsigned char userGameInfoLength; /* 0x4a */
        unsigned char attribute;          /* 0x4b */
        unsigned short parentMaxSize;     /* 0x4c */
        unsigned short childMaxSize;      /* 0x4e */
    } gameInfo;
    unsigned char pad_50[0xc0 - 0x50];
} WMBssDesc;

typedef char WMBssDesc_size_must_be_0xc0[sizeof(WMBssDesc) == 0xc0 ? 1 : -1];

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

/* NitroSDK WMPortRecvCallback, the leading fields: the offsets agree with
 * every port receive callback that reads one (WM_Ds.cpp's two data-sharing
 * receive callbacks and the file-sharing one, func_020417d0). */
typedef struct WMPortRecvCallback {
    unsigned short apiid;        /* 0x00 */
    unsigned short errcode;      /* 0x02 */
    unsigned short state;        /* 0x04 -- 7 CONNECTED, 9 DISCONNECTED, 0x15 PORT_RECV */
    unsigned short port;         /* 0x06 */
    void *recvBuf;               /* 0x08 */
    unsigned short *data;        /* 0x0c */
    unsigned short length;       /* 0x10 -- bytes at data */
    unsigned short aid;          /* 0x12 -- the sender */
    unsigned char macAddress[6]; /* 0x14 */
    unsigned short seqNo;        /* 0x1a */
    void *arg;                   /* 0x1c -- WM_SetPortCallback's argument */
} WMPortRecvCallback;

/* NitroSDK WMDataSet, one data-sharing frame (0x100 bytes): a 4-byte header,
 * then each member's dataLength bytes in AID order. */
typedef struct WMDataSet {
    unsigned short aidHeader;      /* 0x00 -- members whose data the set holds */
    unsigned short receivedBitmap; /* 0x02 -- members whose data arrived */
    unsigned short data[0x7e];     /* 0x04 */
} WMDataSet;

typedef char WMDataSet_size_must_be_0x100[sizeof(WMDataSet) == 0x100 ? 1 : -1];

/* NitroSDK WMDataSharingInfo (0x420; WM_StartDataSharing clears that many
 * bytes): four data sets in a ring, then the ring indices. writeIndex is the
 * set being filled, sendIndex the next to send and readIndex the next
 * WM_StepDataSharing hands over. */
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

typedef char WMDataSharingInfo_size_must_be_0x420[sizeof(WMDataSharingInfo) == 0x420 ? 1 : -1];

/* The SDK-private calls every WM_* wrapper makes; NitroSDK spells them
 * WMi_GetSystemWork, WMi_CheckStateEx, WMi_SendCommand and
 * WMi_SetCallbackTable. The check and the send are variadic: paramNum
 * states to accept, or paramNum words to copy into the request block.
 * WM_SetCallbackTable stores a request's callback in callbackTable by API
 * id. */
#ifdef __cplusplus
extern "C" {
#endif
WMArm9Buf *WM_GetSystemWork(void);
int WM_CheckStateEx(int paramNum, ...);
int WM_SendCommand(short id, unsigned short paramNum, ...);
void WM_SetCallbackTable(int apiid, void *callback);

/* NitroSDK wm_ds.c (WM_Ds.cpp): data sharing. This SDK build takes the
 * port, bitmap, length and mode as ints. */
int WM_StartDataSharing(WMDataSharingInfo *, int, int, int, int);
int WM_StepDataSharing(WMDataSharingInfo *, const void *, WMDataSet *);
unsigned short *WM_GetSharedDataAddress(WMDataSharingInfo *, WMDataSet *, int);
#ifdef __cplusplus
}
#endif

#endif
