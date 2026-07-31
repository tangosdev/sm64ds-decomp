#include "types.h"
extern u32 _ZN3IRQ11SetIRQStateEb_false(void); /* 0x0206da18 */
extern u32 _ZN3IRQ11SetFIQStateEb_false(void); /* 0x0206d9cc */
extern u32 _ZN4CP1517MPUGetDataRegion7Ev(void); /* 0x0206daa4 */
extern void _ZN4CP1514MPUDataRegion7Ej(u32 val); /* 0x0206daac */
typedef struct { u32 irqState; u32 fiqState; u32 mpuRegion7; } SavedIntState;
void func_0206d890(SavedIntState *state)
{
    state->irqState   = _ZN3IRQ11SetIRQStateEb_false();
    state->fiqState   = _ZN3IRQ11SetFIQStateEb_false();
    state->mpuRegion7 = _ZN4CP1517MPUGetDataRegion7Ev();
    _ZN4CP1514MPUDataRegion7Ej(0x02600029);
}
