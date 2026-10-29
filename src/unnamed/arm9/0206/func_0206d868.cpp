//cpp
#include "CP15.h"

extern "C" {

/* local extern: IRQ::SetFIQState(bool) and IRQ::SetIRQState(bool) are entry
 * labels inside func_0206d9cc and func_0206da18. This caller hands them the
 * saved word unnormalised; a bool parameter adds the `!= 0` the ROM lacks. */
extern void _ZN3IRQ11SetFIQStateEb(int state);
extern void _ZN3IRQ11SetIRQStateEb(int state);

typedef struct {
    int irqState;
    int fiqState;
    unsigned int mpuDataRegion7;
} SavedSystemState;

void func_0206d868(const SavedSystemState* s)
{
    CP15::MPUDataRegion7(s->mpuDataRegion7);
    _ZN3IRQ11SetFIQStateEb(s->fiqState);
    _ZN3IRQ11SetIRQStateEb(s->irqState);
}
}
