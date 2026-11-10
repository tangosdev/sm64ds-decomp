//cpp
#include "G2.h"

extern "C" {

extern void MIi_CpuClear16(unsigned short value, void *dst, int count);

void ResetCrashScreen(void)
{
    void *ptr = G2::GetBG1ScrPtr();
    volatile unsigned short val = 0xff;
    MIi_CpuClear16(val, ptr, 0x800);
}
}
