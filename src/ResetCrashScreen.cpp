//cpp
#include "G2.h"

extern "C" {

extern void MultiStore16(unsigned short value, void *dst, int count);

void ResetCrashScreen(void)
{
    void *ptr = G2::GetBG1ScrPtr();
    volatile unsigned short val = 0xff;
    MultiStore16(val, ptr, 0x800);
}
}
