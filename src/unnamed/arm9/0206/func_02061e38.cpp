//cpp
#include "CP15.h"

extern "C" {

extern int WM_CheckStateEx(int a, int b);
extern void WM_SetCallbackTable(int i, int val);
extern int *WM_GetSystemWork(void);
extern void CpuCopy8(void *src, void *dst, unsigned int size);
extern void func_0205a588(void *dst, int v, unsigned int size);
extern int WM_SendCommand(int a, int b);

/* NitroSDK WM_StartConnect(callback, pInfo, ssid): func_020690b0 passes it
 * its connectBssDesc (a 0xc0-byte WMBssDesc, flushed here) and no ssid. The
 * request goes to WM_GetSystemWork's fifo9to7 (word 3), as in func_02061ee4.
 * Only the request fields written here are spelled out. */
typedef struct WMStartConnectReq {
    unsigned short apiid;     /* 0x00 -- WM_APIID_START_CONNECT, 0xc */
    unsigned short reserved;  /* 0x02 */
    int pInfo;                /* 0x04 -- the parent's WMBssDesc */
    unsigned char ssid[0x18]; /* 0x08 -- WM_SIZE_CHILD_SSID; zeroed when none */
} WMStartConnectReq;

typedef char WMStartConnectReq_size_must_be_0x20[sizeof(WMStartConnectReq) == 0x20 ? 1 : -1];

int func_02061e38(int a, int b, void *c)
{
    int r;
    int *p;
    WMStartConnectReq *q;
    r = WM_CheckStateEx(1, 2);
    if (r != 0) return r;
    if (b == 0) return 6;
    CP15::FlushDataCache(b, 0xc0);
    WM_SetCallbackTable(0xc, a);
    p = WM_GetSystemWork();
    q = (WMStartConnectReq *)p[3];
    q->apiid = 0xc;
    q->pInfo = b;
    if (c != 0) {
        CpuCopy8(c, q->ssid, 0x18);
    } else {
        func_0205a588(q->ssid, 0, 0x18);
    }
    r = WM_SendCommand(0xc, 0);
    return (r == 0) ? 2 : r;
}
}
