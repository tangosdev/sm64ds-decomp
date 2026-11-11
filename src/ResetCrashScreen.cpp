//cpp
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/mi/memory.h"

extern "C" {

void ResetCrashScreen(void)
{
    void *ptr = G2_GetBG1ScrPtr();
    volatile unsigned short val = 0xff;
    MIi_CpuClear16(val, ptr, 0x800);
}
}
