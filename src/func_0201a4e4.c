#include "types.h"
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(u32 irqBit, void (*handler)(void));
extern void _ZN3IRQ13VBlankHandlerEv(void);

extern u16 unk_0209d4fc;
extern u16 unk_0209d500;
extern u8  unk_0209d4dc;

/* func_0201a4e4 - initialize VBlank IRQ handler and clear some counters */
void func_0201a4e4(void)
{
    unk_0209d4fc = 0;
    unk_0209d500 = 0;
    _ZN3IRQ13SetIRQHandlerEjPFvvE(1, _ZN3IRQ13VBlankHandlerEv);
    unk_0209d4dc = 1;
}