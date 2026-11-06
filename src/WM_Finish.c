#include "types.h"
#include "private/wm_views.h"
extern int WMi_CheckInitialized(void);
extern void WmClearFifoRecvFlag(void);
extern void PXI_SetFifoRecvCallback(int tag, void *callback);

extern WMArm9Buf *data_020a89ac;
extern u16 data_020a89a8;

/* NitroSDK wm_system.c WM_Finish: once ARM7 has ended WM (WM_STATE_READY),
 * drop the PXI receive callback and forget the WM system buffer.
 * ILLEGAL_STATE (3) before WM_Init. */
int WM_Finish(void) {
    int r;
    if (WMi_CheckInitialized() != 0)
        return 3;
    r = WM_CheckStateEx(1, 0);
    if (r != 0)
        return r;
    WmClearFifoRecvFlag();
    PXI_SetFifoRecvCallback(0xa, 0);
    data_020a89ac = 0;
    data_020a89a8 = 0;
    return 0;
}
