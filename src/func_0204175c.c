#include "types.h"
/* Hardware IO registers */
static volatile u32 * const REG_IE = (volatile u32 *)0x04000210;
static volatile u32 * const REG_IF = (volatile u32 *)0x04000214;

/* Globals */
extern void (*gGameCardHandlerPtr)(void *);  /* *0x0209a03c */
extern void *gGameCardHandlerArg;            /* 0x02099e24 */
extern u32   gGameCardSentinel;              /* 0x020a0f38 */

extern u32  _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);
extern void _ZN3IRQ10EnableIRQsEj(u32 irqBits);

void func_0204175c(void)
{
    u32 state;
    u32 ie;
    u32 iflags;

    state = _ZN3IRQ7DisableEv();
    ie     = *REG_IE;
    iflags = *REG_IF;

    if ((iflags & 0x80000) && !(ie & 0x80000))
    {
        _ZN3IRQ10EnableIRQsEj(0x80000);
        gGameCardHandlerPtr(&gGameCardHandlerArg);
        gGameCardSentinel = 0x80000000;
    }

    _ZN3IRQ7RestoreEj(state);
}
