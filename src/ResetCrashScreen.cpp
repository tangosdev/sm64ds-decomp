//cpp
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/mi/memory.h"


void ResetCrashScreen(void)
{
    void *ptr = G2_GetBG1ScrPtr();
    volatile unsigned short val = 0xff;
    MI_CpuFill16(ptr, val, 0x800);
}
