//cpp
#include "IRQ.h"

extern "C" {

void func_02059f58(int channel)
{
    unsigned int saved = IRQ::Disable();
    int idx = channel * 6 + 5;
    volatile unsigned short *reg = (volatile unsigned short *)(0x40000b0 + idx * 2);
    *reg &= ~0x3a00;
    *reg &= ~0x8000;
    (void)*reg;
    (void)*reg;
    IRQ::Restore(saved);
}
}
