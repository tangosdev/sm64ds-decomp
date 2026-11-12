//cpp
#include "nitro/os/cache.h"
#include "private/wm_views.h"
#include "nitro/mi/memory.h"

extern "C" {

/* NitroSDK WM_StartConnect(callback, pInfo, ssid): MBi_ChildCallback passes it
 * its connectBssDesc (a 0xc0-byte WMBssDesc, flushed here) and no ssid. The
 * request goes to WM_GetSystemWork's fifo9to7 (word 3), as in WM_StartScan.
 * Only the request fields written here are spelled out. */
typedef struct WMStartConnectReq {
    unsigned short apiid;     /* 0x00 -- WM_APIID_START_CONNECT, 0xc */
    unsigned short reserved;  /* 0x02 */
    const WMBssDesc *pInfo;   /* 0x04 */
    unsigned char ssid[0x18]; /* 0x08 -- WM_SIZE_CHILD_SSID; zeroed when none */
} WMStartConnectReq;

typedef char WMStartConnectReq_size_must_be_0x20[sizeof(WMStartConnectReq) == 0x20 ? 1 : -1];

int WM_StartConnect(void *callback, const WMBssDesc *pInfo, const u8 *ssid) {
    int r;
    WMArm9Buf *p;
    WMStartConnectReq *q;
    r = WM_CheckStateEx(1, 2);
    if (r != 0) return r;
    if (pInfo == 0)
        return 6;
    DC_StoreRange((void *)pInfo, sizeof(WMBssDesc));
    WM_SetCallbackTable(0xc, callback);
    p = WM_GetSystemWork();
    q = (WMStartConnectReq *)p->fifo9to7;
    q->apiid = 0xc;
    q->pInfo = pInfo;
    if (ssid != 0) {
        MI_CpuCopy8(ssid, q->ssid, 0x18);
    } else {
        MI_CpuClear8(q->ssid, 0x18);
    }
    r = WM_SendCommand(0xc, 0);
    return (r == 0) ? 2 : r;
}
}
