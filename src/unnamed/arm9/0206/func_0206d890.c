#include "types.h"
extern int func_0206da18(void); /* IRQ::SetIRQState(false) entry */
extern int func_0206d9cc(void); /* IRQ::SetFIQState(false) entry */
extern u32 _ZN4CP1517MPUGetDataRegion7Ev(void); /* 0x0206daa4 */
extern void _ZN4CP1514MPUDataRegion7Ej(u32 val); /* 0x0206daac */
typedef struct { u32 irqState; u32 fiqState; u32 mpuRegion7; } SavedIntState;
void func_0206d890(SavedIntState *state)
{
    state->irqState   = func_0206da18();
    state->fiqState   = func_0206d9cc();
    state->mpuRegion7 = _ZN4CP1517MPUGetDataRegion7Ev();
    _ZN4CP1514MPUDataRegion7Ej(0x02600029);
}
