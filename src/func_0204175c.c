#include "types.h"
/* Hardware IO registers */
static volatile u32 * const REG_IE = (volatile u32 *)0x04000210;
static volatile u32 * const REG_IF = (volatile u32 *)0x04000214;

/* Globals */
extern void (*data_0209a03c)(void *);  /* *0x0209a03c */
extern void *data_02099e24;            /* 0x02099e24 */
extern u32   data_020a0f38;              /* 0x020a0f38 */

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
        data_0209a03c(&data_02099e24);
        data_020a0f38 = 0x80000000;
    }

    _ZN3IRQ7RestoreEj(state);
}
