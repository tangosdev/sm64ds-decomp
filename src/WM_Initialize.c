/* NitroSDK wm_standard.c WM_Initialize: READY to IDLE in one step. Run
 * WM_Init on wmSysBuf with DMA channel dmaNo, register callback for API 0
 * (INITIALIZE) and send that command with the three buffers WM_Enable
 * sends; ARM7 then enables and powers on the hardware itself.
 * WM_ERRCODE_OPERATING (2) once it is queued. */
extern int WM_Init(int a, int b);
#include "private/wm_views.h"

int WM_Initialize(int wmSysBuf, void *callback, int dmaNo) {
    WMArm9Buf *sys;
    int result;

    result = WM_Init(wmSysBuf, dmaNo);
    if (result != 0)
        return result;
    WM_SetCallbackTable(0, callback);
    sys = WM_GetSystemWork();
    result = WM_SendCommand(0, 3, sys->WM7, sys->status, sys->fifo7to9);
    if (result != 0)
        return result;
    return 2;
}
