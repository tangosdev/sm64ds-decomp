#ifndef NITRO_WM_WM_H
#define NITRO_WM_WM_H

/*
 * The Nitro SDK's nitro/wm/common/wm.h, the part of it this tree uses so far.
 *
 * WMApiid is listed up to MEASURE_CHANNEL (0x1e): every WM_* wrapper in src/
 * sends the id the SDK gives its name, up to WM_MeasureChannel's 0x1e.
 * Later SDKs add ids past it; this one's callback table has 0x25 slots, so it
 * has some of them too, but not under names any body here confirms.
 * WMErrCode, WMState and WMPriority are the SDK's values, and the ROM's
 * code agrees with them: MP data needs MP_PARENT 9 or MP_CHILD 0xa, a bad
 * buffer is INVALID_PARAM 6 and a parent without children NO_CHILD 7, and
 * data sharing sends at HIGH 1 where multiboot sends at LOW 3.
 *
 * WMCallbackFunc is the callback every asynchronous WM call takes. WM hands
 * it the call's result record, so callbacks take a void * and cast it.
 *
 * WMBssDesc, WMParentParam and WMScanParam are this SDK revision's layouts,
 * which differ from the public header where each says so. Plain C types:
 * some consumers do not include types.h.
 */

#ifdef __cplusplus
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
    WM_APIID_MEASURE_CHANNEL
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
    WM_ERRCODE_SEND_FAILED = 15
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

typedef enum WMPriority {
    WM_PRIORITY_URGENT = 0,
    WM_PRIORITY_HIGH,
    WM_PRIORITY_NORMAL,
    WM_PRIORITY_LOW
} WMPriority;

typedef void (*WMCallbackFunc)(void *);

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

#ifdef __cplusplus
}
#endif

#endif
