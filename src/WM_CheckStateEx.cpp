//cpp
#include "types.h"
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

extern int WMi_CheckInitialized(void);
extern WMArm9Buf *data_020a89ac;

int WM_CheckStateEx(int count, ...)
{
    int r;
    int *p;
    u16 target;
    r = WMi_CheckInitialized();
    if (r != 0)
        return r;
    CP15::InvalidateDataCache((unsigned int)data_020a89ac->status, 2);
    target = data_020a89ac->status->state;
    p = (int *)((unsigned int)&count & ~3) + 1;
    r = 3;
    if (count == 0)
        return 3;
    do {
        p++;
        if (p[-1] == target)
            r = 0;
    } while (--count != 0);
    return r;
}
}
