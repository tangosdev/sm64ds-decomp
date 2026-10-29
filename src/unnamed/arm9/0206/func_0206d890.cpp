//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

extern int func_0206da18(void); /* IRQ::SetIRQState(false) entry */
extern int func_0206d9cc(void); /* IRQ::SetFIQState(false) entry */
typedef struct { u32 irqState; u32 fiqState; u32 mpuRegion7; } SavedIntState;
void func_0206d890(SavedIntState *state)
{
    state->irqState   = func_0206da18();
    state->fiqState   = func_0206d9cc();
    state->mpuRegion7 = CP15::MPUGetDataRegion7();
    CP15::MPUDataRegion7(0x02600029);
}
}
