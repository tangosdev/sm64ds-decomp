//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "private/wm_views.h"

extern "C" {

extern int WMi_CheckInitialized(void);

/* NitroSDK wm_sync.c WM_GetLinkLevel, by body against pret's pokediamond
 * WM_sync.s: the radio link level, or 0 unless MP or DCF is running, as a
 * parent with at least one child or as a child. Each status field is
 * invalidated from the data cache before it is read. */
int WM_GetLinkLevel(void) {
    WMArm9Buf *sys = WM_GetSystemWork();
    WMStatus *status;
    if (WMi_CheckInitialized())
        return 0;
    DC_InvalidateRange(&sys->status->state, 2);
    status = sys->status;
    switch (status->state) {
    case 9:
        DC_InvalidateRange(&status->child_bitmap, 2);
        status = sys->status;
        if (status->child_bitmap == 0)
            return 0;
        /* fall through */
    case 0xa:
    case 0xb:
        DC_InvalidateRange(&status->linkLevel, 2);
        return sys->status->linkLevel;
    }
    return 0;
}
}
