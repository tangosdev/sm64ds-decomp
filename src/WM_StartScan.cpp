//cpp
#include "private/wm_views.h"

namespace CP15 {
void FlushDataCache(unsigned int a, unsigned int b);
}
extern "C" {
extern int WM_CheckStateEx(int count, ...);

extern void WM_SetCallbackTable(int i, int val);
extern char *WM_GetSystemWork(void);
extern int WM_SendCommand(short a0, unsigned short count);

/* NitroSDK WMScanParam, WM_StartScan's argument (same names as
 * func_020690b0's WmScanParam, which passes data_020a9dc0 here). */
struct WmScanParam {
    void *scanBuf;                 /* 0x0 */
    unsigned short channel;        /* 0x4 -- 1..14 */
    unsigned short maxChannelTime; /* 0x6 */
    unsigned short bssid[3];       /* 0x8: SDK u8[6] */
};

/* NitroSDK WMStartScanReq: the request WM_StartScan writes for ARM7. */
struct WMStartScanReq {
    unsigned short apiid;          /* 0x0 -- WM_APIID_START_SCAN, 0xa */
    unsigned short channel;        /* 0x2 */
    void *scanBuf;                 /* 0x4 */
    unsigned short maxChannelTime; /* 0x8 */
    unsigned short bssid[3];       /* 0xa */
};

/* NitroSDK WM_StartScan(callback, param): in state IDLE, CLASS1 or SCAN
 * (2, 3, 5), check the scan parameter (a buffer and channel 1..14), flush
 * it, register the callback and send the START_SCAN request. Returns 2
 * (WM_ERRCODE_OPERATING) once the request is queued. */
int WM_StartScan(int a, char *b) {
    WmScanParam *param = (WmScanParam *)b;
    int rc;
    WMStartScanReq *req;
    unsigned short h;
    rc = WM_CheckStateEx(3, 2, 3, 5);
    if (rc != 0) return rc;
    if (b == 0) return 6;
    if (param->scanBuf == 0)
        return 6;
    h = param->channel;
    if (h < 1 || h > 0xe) return 6;
    CP15::FlushDataCache((unsigned int)b, 0x20);
    WM_SetCallbackTable(0xa, a);
    req = (WMStartScanReq *)((WMArm9Buf *)WM_GetSystemWork())->fifo9to7;
    req->apiid = 0xa;
    req->channel = param->channel;
    req->scanBuf = param->scanBuf;
    req->maxChannelTime = param->maxChannelTime;
    req->bssid[0] = param->bssid[0];
    req->bssid[1] = param->bssid[1];
    req->bssid[2] = param->bssid[2];
    rc = WM_SendCommand(0xa, 0);
    if (rc == 0) return 2;
    return rc;
}
}
