#include "types.h"
extern u32  _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(u32 irqBits, void (*handler)(void));
extern void _ZN3IRQ10EnableIRQsEj(u32 irqBits);
extern void _ZN3IRQ21GameCardIREQMCHandlerEv(void);

void func_02061138(void)
{
    u32 state;

    state = _ZN3IRQ7DisableEv();
    _ZN3IRQ13SetIRQHandlerEjPFvvE(0x100000, _ZN3IRQ21GameCardIREQMCHandlerEv);
    _ZN3IRQ10EnableIRQsEj(0x100000);
    _ZN3IRQ7RestoreEj(state);
}
