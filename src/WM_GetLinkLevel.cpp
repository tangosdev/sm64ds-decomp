//cpp
#include "types.h"
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

extern void *WM_GetSystemWork(void);
extern int WMi_CheckInitialized(void);

/* NitroSDK wm_sync.c WM_GetLinkLevel, by body against pret's pokediamond
 * WM_sync.s: the radio link level, or 0 unless MP or DCF is running, as a
 * parent with at least one child or as a child. Each status field is
 * invalidated from the data cache before it is read. */
int WM_GetLinkLevel(void) {
    WMArm9Buf *sys = (WMArm9Buf *)WM_GetSystemWork();
    WMStatus *status;
    if (WMi_CheckInitialized())
        return 0;
    CP15::InvalidateDataCache((unsigned int)&sys->status->state, 2);
    status = sys->status;
    switch (status->state) {
    case 9:
        CP15::InvalidateDataCache((unsigned int)&status->child_bitmap, 2);
        status = sys->status;
        if (status->child_bitmap == 0)
            return 0;
        /* fall through */
    case 0xa:
    case 0xb:
        CP15::InvalidateDataCache((unsigned int)&status->linkLevel, 2);
        return sys->status->linkLevel;
    }
    return 0;
}
}
