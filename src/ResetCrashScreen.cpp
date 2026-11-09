//cpp
#include "nitro/gx/gx_bgcnt.h"

extern "C" {

extern void MultiStore16(unsigned short value, void *dst, int count);

void ResetCrashScreen(void)
{
    void *ptr = G2_GetBG1ScrPtr();
    volatile unsigned short val = 0xff;
    MultiStore16(val, ptr, 0x800);
}
}
