#include "types.h"
struct DMAIRQEntry {
    u32 handler;    /* +0 */
    u32 active;     /* +4 */
    u32 arg;        /* +8 */
};

extern struct DMAIRQEntry gDMAIRQTable[];   /* 0x020a60c4 */

extern u32 _ZN3IRQ10EnableIRQsEj(u32 mask);

void func_02056e98(u32 idx, u32 handler, u32 arg)
{
    u32 mask = 1u << (idx + 8);
    gDMAIRQTable[idx].handler = handler;
    gDMAIRQTable[idx].arg = arg;
    u32 prev_ie = _ZN3IRQ10EnableIRQsEj(mask);
    gDMAIRQTable[idx].active = prev_ie & (1u << (idx + 8));
}
