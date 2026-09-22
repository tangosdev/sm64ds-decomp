//cpp
// @symbol _ZN7dWipe_c20IsBetweenStartAndEndEv
/* recovered: real C++ member function -- the compiler spells the symbol
 *
 * dWipe_c::IsBetweenStartAndEnd, vtable slot 7. `type == 1` is this class's
 * escape hatch: the hardware capture path is not in use, so the question goes
 * straight back to the base implementation through a QUALIFIED call --
 * FaderBrightness::IsBetweenStartAndEnd() -- which mwcc turns into a direct
 * `bl`, which is exactly what the ROM does. An unqualified call would dispatch
 * through this object's own vptr and land back here.
 *
 * Otherwise the answer is composed from this object's own slots 5 and 6, both
 * of which dWipe_c overrides, so those two stay virtual dispatches.
 */
#include "dWipe_c.h"

int dWipe_c::IsBetweenStartAndEnd()
{
    if (type == 1)
        return FaderBrightness::IsBetweenStartAndEnd();

    if (!IsAtStart()) {
        if (IsAtEnd() == 0)
            return 1;
    }
    return 0;
}
