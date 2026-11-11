//cpp
/* NitroSDK wm_sync.c WM_ReadStatus: copy the status block ARM7 shares
 * (0x930 bytes, invalidated from the data cache first) into statusBuf.
 * WM_ERRCODE_ILLEGAL_STATE (3) before WM_Init, WM_ERRCODE_INVALID_PARAM
 * (6) for a null statusBuf, else WM_ERRCODE_SUCCESS. */
#include "types.h"
#include "nitro/os/cache.h"
#include "private/wm_views.h"
#include "nitro/mi/memory.h"

extern "C" {

extern int WMi_CheckInitialized(void);

int WM_ReadStatus(int *statusBuf) {
    WMArm9Buf *sys = WM_GetSystemWork();
    int result = WMi_CheckInitialized();
    if (result != 0)
        return result;
    if (statusBuf == 0)
        return 6;
    DC_InvalidateRange(sys->status, 0x930);
    MI_CpuCopyFast(sys->status, statusBuf, 0x930);
    return 0;
}
}
