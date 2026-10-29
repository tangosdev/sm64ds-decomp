//cpp
#include "G2.h"

extern "C" {

extern void MultiStore16(unsigned short value, volatile void* dst, int count);

void ResetCrashScreen(void)
{
    volatile void *ptr = G2::GetBG1ScrPtr();
    volatile unsigned short val = 0xff;
    MultiStore16(val, ptr, 0x800);
}
}
