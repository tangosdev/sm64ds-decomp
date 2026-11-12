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

#ifdef __cplusplus
}
#endif

#endif
