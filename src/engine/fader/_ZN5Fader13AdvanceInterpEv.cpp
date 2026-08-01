//cpp
// @symbol _ZN5Fader13AdvanceInterpEv
/* recovered: named members + shared header, real C++ method */
#include "Fader.h"

extern "C" void func_0203ae58(Fix12i* value, Fix12i target, Fix12i step);

/* Step currInterp one frame toward its target. A positive speed fades toward
   1.0, a negative one toward 0.0; the helper takes an unsigned step. */
void Fader::AdvanceInterp()
{
    Fix12i step = speed;
    Fix12i target = step >= 0 ? 0x1000 : 0;
    if (step < 0)
        step = -step;
    func_0203ae58(&currInterp, target, step);
}
