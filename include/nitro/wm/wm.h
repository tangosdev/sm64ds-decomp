#ifndef NITRO_WM_WM_H
#define NITRO_WM_WM_H

/*
 * The Nitro SDK's nitro/wm/common/wm.h, the part of it this tree uses so far.
 *
 * WMApiid is listed up to MEASURE_CHANNEL (0x1e): every WM_* wrapper in src/
 * sends the id the SDK gives its name, up to WM_MeasureChannel's 0x1e.
 * Later SDKs add ids past it; this one's callback table has 0x25 slots, so it
 * has some of them too, but not under names any body here confirms. The ids
 * ARM7 sends outside any request, INDICATION 0x80 and the two port ids after
 * it, are the ones WmReceiveFifo tests; READ_STATUS 0x83 is WM_ReadStatus's.
 * WMErrCode, WMState, WMStateCode and WMPriority are the SDK's values, and the ROM's
 * code agrees with them: MP data needs MP_PARENT 9 or MP_CHILD 0xa, a bad
 * buffer is INVALID_PARAM 6 and a parent without children NO_CHILD 7, and
 * data sharing sends at HIGH 1 where multiboot sends at LOW 3.
 *
 * WMCallbackFunc is the callback every asynchronous WM call takes. WM hands
 * it the call's result record, so callbacks take a void * and cast it. The
 * records past WMCallback are declared as far as the SDK's public header
 * and this SDK's code agree.
 *
 * WMBssDesc, WMParentParam, WMScanParam and WMStatus are this SDK
 * revision's layouts, which differ from the public header where each says so.
 * The MP receive records are the public header's: the buffer sizes
 * WM_GetMPReceiveBufferSize computes from their sizeof are the ROM's. Plain C
 * types: some consumers do not include types.h.
 */

#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif

typedef enum WMApiid {
    WM_APIID_INITIALIZE = 0,
    WM_APIID_RESET,
    WM_APIID_END,
    WM_APIID_ENABLE,
    WM_APIID_DISABLE,
    WM_APIID_POWER_ON,
    WM_APIID_POWER_OFF,
    WM_APIID_SET_P_PARAM,
    WM_APIID_START_PARENT,
    WM_APIID_END_PARENT,
    WM_APIID_START_SCAN,
    WM_APIID_END_SCAN,
    WM_APIID_START_CONNECT,
    WM_APIID_DISCONNECT,
    WM_APIID_START_MP,
    WM_APIID_SET_MP_DATA,
    WM_APIID_END_MP,
    WM_APIID_START_DCF,
    WM_APIID_SET_DCF_DATA,
    WM_APIID_END_DCF,
    WM_APIID_SET_WEPKEY,
    WM_APIID_START_KS,
    WM_APIID_END_KS,
    WM_APIID_GET_KEYSET,
    WM_APIID_SET_GAMEINFO,
    WM_APIID_SET_BEACON_IND,
    WM_APIID_START_TESTMODE,
    WM_APIID_STOP_TESTMODE,
    WM_APIID_VALARM_MP,
    WM_APIID_SET_LIFETIME,
    WM_APIID_MEASURE_CHANNEL,
    WM_APIID_INDICATION = 0x80,
    WM_APIID_PORT_SEND,
    WM_APIID_PORT_RECV,
    WM_APIID_READ_STATUS
} WMApiid;

typedef enum WMErrCode {
    WM_ERRCODE_SUCCESS = 0,
    WM_ERRCODE_FAILED = 1,
    WM_ERRCODE_OPERATING = 2,
    WM_ERRCODE_ILLEGAL_STATE = 3,
    WM_ERRCODE_WM_DISABLE = 4,
    WM_ERRCODE_NO_KEYSET = 5,
    WM_ERRCODE_NO_DATASET = 5,
    WM_ERRCODE_INVALID_PARAM = 6,
    WM_ERRCODE_NO_CHILD = 7,
    WM_ERRCODE_FIFO_ERROR = 8,
    WM_ERRCODE_TIMEOUT = 9,
    WM_ERRCODE_SEND_QUEUE_FULL = 10,
    WM_ERRCODE_NO_ENTRY = 11,
    WM_ERRCODE_OVER_MAX_ENTRY = 12,
    WM_ERRCODE_INVALID_POLLBITMAP = 13,
    WM_ERRCODE_NO_DATA = 14,
    WM_ERRCODE_SEND_FAILED = 15,
    WM_ERRCODE_DCF_TEST,
    WM_ERRCODE_WL_INVALID_PARAM,
    WM_ERRCODE_WL_LENGTH_ERR,
    WM_ERRCODE_FLASH_ERROR
} WMErrCode;

typedef enum WMState {
    WM_STATE_READY = 0,
    WM_STATE_STOP,
    WM_STATE_IDLE,
    WM_STATE_CLASS1,
    WM_STATE_TESTMODE,
    WM_STATE_SCAN,
    WM_STATE_CONNECT,
    WM_STATE_PARENT,
    WM_STATE_CHILD,
    WM_STATE_MP_PARENT,
    WM_STATE_MP_CHILD,
    WM_STATE_DCF_CHILD
} WMState;

/* The state a callback reports (its result record's `state`). */
typedef enum WMStateCode {
    WM_STATECODE_PARENT_START = 0,
    WM_STATECODE_BEACON_SENT = 2,
    WM_STATECODE_SCAN_START = 3,
    WM_STATECODE_PARENT_NOT_FOUND = 4,
    WM_STATECODE_PARENT_FOUND = 5,
    WM_STATECODE_CONNECT_START = 6,
    WM_STATECODE_BEACON_LOST = 8,
    WM_STATECODE_CONNECTED = 7,
    WM_STATECODE_CHILD_CONNECTED = 7,
    WM_STATECODE_DISCONNECTED = 9,
    WM_STATECODE_DISCONNECTED_FROM_MYSELF = 26,
    WM_STATECODE_MP_START = 10,
    WM_STATECODE_MPEND_IND = 11,
    WM_STATECODE_MP_IND = 12,
    WM_STATECODE_MPACK_IND = 13,
    WM_STATECODE_DCF_START = 14,
    WM_STATECODE_DCF_IND = 15,
    WM_STATECODE_BEACON_RECV = 16,
    WM_STATECODE_DISASSOCIATE = 17,
    WM_STATECODE_REASSOCIATE = 18,
    WM_STATECODE_AUTHENTICATE = 19,
    WM_STATECODE_PORT_INIT = 25,
    WM_STATECODE_PORT_SEND = 20,
    WM_STATECODE_PORT_RECV = 21,
    WM_STATECODE_FIFO_ERROR = 22,
    WM_STATECODE_INFORMATION = 23,
    WM_STATECODE_UNKNOWN = 24
} WMStateCode;

typedef enum WMLinkLevel {
    WM_LINK_LEVEL_0 = 0,
    WM_LINK_LEVEL_1,
    WM_LINK_LEVEL_2,
    WM_LINK_LEVEL_3,
    WM_LINK_LEVEL_MAX
} WMLinkLevel;

/* Data sharing's state (WMDataSharingInfo.state). This SDK predates
 * WM_PauseDataSharing: the later SDK's PAUSING and PAUSED come between
 * START and RETRY_SEND there, and wm_ds.c here sets only 1, 2 and 3. */
typedef enum WMDataSharingState {
    WM_DS_STATE_READY = 0,
    WM_DS_STATE_START,
    WM_DS_STATE_RETRY_SEND,
    WM_DS_STATE_ERROR
} WMDataSharingState;

#define WM_NUM_OF_CALLBACK 0x25
#define WM_NUM_OF_PORT 16

/* The data-sharing set: a 4-byte header, then WM_DS_DATA_SIZE bytes. This
 * build takes the SDK's WM_DS_DATA_SIZE_252 size. */
#define WM_DS_HEADER_SIZE 4
#define WM_DS_DATA_SIZE 252
#define WM_DS_DATASET_NUM 4

/* The beacon's attribute byte (WMParentParam's flags, as WM_SetGameInfo
 * takes them). */
#define WM_ATTR_FLAG_ENTRY 0x0001
#define WM_ATTR_FLAG_MB 0x0002
#define WM_ATTR_FLAG_KS 0x0004
#define WM_ATTR_FLAG_CS 0x0008

typedef enum WMPriority {
    WM_PRIORITY_URGENT = 0,
    WM_PRIORITY_HIGH,
    WM_PRIORITY_NORMAL,
    WM_PRIORITY_LOW
} WMPriority;

typedef void (*WMCallbackFunc)(void *);

/* NitroSDK WMCallback, the head of every result record a WMCallbackFunc
 * receives: the call's API id and its result. */
typedef struct WMCallback {
    unsigned short apiid;    /* 0x0 */
    unsigned short errcode;  /* 0x2 */
    unsigned short wlCmdID;  /* 0x4 */
    unsigned short wlResult; /* 0x6 */
} WMCallback;

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

/* NitroSDK WMParentParam, the parent parameters WM_SetParentParameter hands
 * ARM7 (0x40 bytes). ggid is two u16s at 0x06 with no padding before it, so
 * tgid..childMaxSize sit 2 bytes earlier than in the public NitroSDK header.
 * That is this ROM's SDK revision: MB_Init fills parentParam at these offsets
 * byte-identically before WM_SetParentParameter, and the size assert below
 * holds. Don't realign it to the public header. */
typedef struct WMParentParam {
    unsigned short *userGameInfo;      /* 0x00 */
    unsigned short userGameInfoLength; /* 0x04 */
    unsigned short ggid[2];            /* 0x06 */
    unsigned short tgid;               /* 0x0a */
    unsigned short entryFlag;          /* 0x0c */
    unsigned short maxEntry;           /* 0x0e */
    unsigned short multiBootFlag;      /* 0x10 */
    unsigned short KS_Flag;            /* 0x12 */
    unsigned short CS_Flag;            /* 0x14 */
    unsigned short beaconPeriod;       /* 0x16 */
    unsigned short rsv[12];            /* 0x18 */
    unsigned short channel;            /* 0x30 */
    unsigned short parentMaxSize;      /* 0x32 */
    unsigned short childMaxSize;       /* 0x34 */
    unsigned short rsv2[5];            /* 0x36 */
} WMParentParam;

typedef char WMParentParam_size_must_be_0x40[sizeof(WMParentParam) == 0x40 ? 1 : -1];

/* NitroSDK WMScanParam, WM_StartScan's argument (WM_StartScan flushes 0x20
 * bytes of it). This SDK build copies bssid as three halfwords; the public
 * header has u8[6]. All 0xffff scans for any parent. */
typedef struct WMScanParam {
    WMBssDesc *scanBuf;            /* 0x0 */
    unsigned short channel;        /* 0x4 -- 1..14 */
    unsigned short maxChannelTime; /* 0x6 */
    unsigned short bssid[3];       /* 0x8 */
    unsigned char pad_0e[0x20 - 0xe];
} WMScanParam;

typedef char WMScanParam_size_must_be_0x20[sizeof(WMScanParam) == 0x20 ? 1 : -1];

/* NitroSDK WMMpRecvBuf, the frame a child receives from its parent. */
typedef struct WMMpRecvBuf {
    unsigned short rsv1[3];      /* 0x00 */
    unsigned short length;       /* 0x06 */
    unsigned short rsv2[1];      /* 0x08 */
    unsigned short ackTimeStamp; /* 0x0a */
    unsigned short timeStamp;    /* 0x0c */
    unsigned short rate_rssi;    /* 0x0e */
    unsigned short rsv3[2];      /* 0x10 */
    unsigned short rsv4[2];      /* 0x14 */
    unsigned char destAdrs[6];   /* 0x18 */
    unsigned char srcAdrs[6];    /* 0x1e */
    unsigned short rsv5[3];      /* 0x24 */
    unsigned short seqCtrl;      /* 0x2a */
    unsigned short txop;         /* 0x2c */
    unsigned short bitmap;       /* 0x2e */
    unsigned short wmHeader;     /* 0x30 */
    unsigned short data[2];      /* 0x32 */
} WMMpRecvBuf;

/* NitroSDK WMMpRecvData, one child's block in what a parent receives. */
typedef struct WMMpRecvData {
    unsigned short length;     /* 0x0 */
    unsigned short rate_rssi;  /* 0x2 */
    unsigned short aid;        /* 0x4 */
    unsigned short noResponse; /* 0x6 */
    unsigned short wmHeader;   /* 0x8 */
    unsigned short cdata[1];   /* 0xa */
} WMMpRecvData;

/* NitroSDK WMMpRecvHeader: count blocks of length bytes each, from data. */
typedef struct WMMpRecvHeader {
    unsigned short bitmap;    /* 0x0 */
    unsigned short errBitmap; /* 0x2 */
    unsigned short count;     /* 0x4 */
    unsigned short length;    /* 0x6 */
    unsigned short txCount;   /* 0x8 */
    WMMpRecvData data[1];     /* 0xa */
} WMMpRecvHeader;

typedef char WMMpRecvBuf_size_must_be_0x36[sizeof(WMMpRecvBuf) == 0x36 ? 1 : -1];
typedef char WMMpRecvData_size_must_be_0xc[sizeof(WMMpRecvData) == 0xc ? 1 : -1];
typedef char WMMpRecvHeader_size_must_be_0x16[sizeof(WMMpRecvHeader) == 0x16 ? 1 : -1];

/* NitroSDK WMStartMPCallback, WM_StartMP's record: MP_START once, then an
 * MPEND_IND or MP_IND with the received data in recvBuf for every exchange. */
typedef struct WMStartMPCallback {
    unsigned short apiid;   /* 0x00 */
    unsigned short errcode; /* 0x02 */
    unsigned short state;   /* 0x04 -- a WMStateCode */
    unsigned char reserved[2];
    WMMpRecvBuf *recvBuf;      /* 0x08 */
    unsigned short timeStamp;  /* 0x0c */
    unsigned short rate_rssi;  /* 0x0e */
    unsigned char destAdrs[6]; /* 0x10 */
    unsigned char srcAdrs[6];  /* 0x16 */
    unsigned short seqNum;     /* 0x1c */
    unsigned short tmptt;      /* 0x1e */
    unsigned short pollbmp;    /* 0x20 */
    unsigned short reserved2;  /* 0x22 */
} WMStartMPCallback;

/* NitroSDK WMMeasureChannelCallback, WM_MeasureChannel's record. */
typedef struct WMMeasureChannelCallback {
    unsigned short apiid;        /* 0x0 */
    unsigned short errcode;      /* 0x2 */
    unsigned short wlCmdID;      /* 0x4 */
    unsigned short wlResult;     /* 0x6 */
    unsigned short channel;      /* 0x8 */
    unsigned short ccaBusyRatio; /* 0xa -- percent */
} WMMeasureChannelCallback;

/* NitroSDK WMIndCallback, the record of an INDICATION ARM7 sends outside any
 * request. */
typedef struct WMIndCallback {
    unsigned short apiid;   /* 0x0 */
    unsigned short errcode; /* 0x2 */
    unsigned short state;   /* 0x4 -- a WMStateCode */
    unsigned short reason;  /* 0x6 */
} WMIndCallback;

/* NitroSDK WMStatus, the status block ARM7 keeps and WM_ReadStatus copies
 * (0x930 bytes in this SDK revision). Only the fields some code reads, at
 * the width it reads them, are named; the rest is padding. linkLevel and
 * child_bitmap sit 4 bytes lower than in pret's pokediamond, and pparam is
 * this revision's WMParentParam. */
typedef struct WMStatus {
    unsigned short state; /* 0x000 -- a WMState */
    char pad_002[0x10 - 0x2];
    int mp_flag; /* 0x010 -- 1 once MP has started */
    char pad_014[0x46 - 0x14];
    unsigned short mp_recvBufSize; /* 0x046 -- what WmReceiveFifo invalidates at a recvBuf */
    char pad_048[0x86 - 0x48];
    unsigned short mp_readyBitmap; /* 0x086 -- children the parent's data sets wait for */
    char pad_088[0xb8 - 0x88];
    unsigned short linkLevel; /* 0x0b8 -- a WMLinkLevel */
    char pad_0ba[0xe4 - 0xba];
    WMParentParam pparam; /* 0x0e4 */
    char pad_124[0x17e - 0x124];
    unsigned short child_bitmap; /* 0x17e -- connected children, by AID bit */
    char pad_180[0x184 - 0x180];
    unsigned short aid; /* 0x184 -- 0 for the parent */
    char pad_186[0x18e - 0x186];
    unsigned short mp_maxSendSize; /* 0x18e */
    unsigned short mp_maxRecvSize; /* 0x190 */
    char pad_192[0x1f2 - 0x192];
    unsigned short allowedChannel; /* 0x1f2 -- bit n-1 set when channel n is usable */
    char pad_1f4[0x930 - 0x1f4];
} WMStatus;

typedef char WMStatus_size_must_be_0x930[sizeof(WMStatus) == 0x930 ? 1 : -1];

#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#endif
