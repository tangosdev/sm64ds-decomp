#include "types.h"
struct TimerIRQEntry {
    u32 handler;    /* +0 */
    u32 active;     /* +4 */
    u32 arg;        /* +8 */
};

extern struct TimerIRQEntry data_020a60f4[];  /* 0x020a60f4 */

extern void _ZN3IRQ10EnableIRQsEj(u32 mask);

void func_02056e4c(u32 idx, u32 handler, u32 arg)
{
    data_020a60f4[idx].handler = handler;
    data_020a60f4[idx].arg = arg;
    _ZN3IRQ10EnableIRQsEj(1u << (idx + 3));
    data_020a60f4[idx].active = 1;
}
