//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

extern volatile u32 data_020a6438[2];

long long func_02059650(void)
{
    u32 tok = IRQ::Disable();
    volatile u16 timer = *(volatile u16*)0x4000100;
    volatile unsigned long long counter = *(volatile unsigned long long*)data_020a6438 & 0xffffffffffffULL;
    if ((*(volatile u32*)0x4000214 & 8) && !(timer & 0x8000))
        counter = counter + 1;
    IRQ::Restore(tok);
    return ((long long)counter << 16) | (int)(timer);
}
}
