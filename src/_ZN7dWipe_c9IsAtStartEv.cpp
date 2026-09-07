//cpp
// @symbol _ZN7dWipe_c9IsAtStartEv
/* recovered: named members + real C++ method */
/* dWipe_c::IsAtStart() -- vtable slot 5. Type 1 is the plain brightness fade,
 * which the base answers. Otherwise a wipe is "at start" whenever its state
 * machine is idle, or its own 20.12 ramp has not passed the halfway mark. */
#include "dWipe_c.h"

int dWipe_c::IsAtStart()
{
    int result;
    if (type == 1)
        return FaderBrightness::IsAtStart();
    if (state == 0)
        goto ret1;
    if (wipeInterp < 0x200000)
        goto ret0;
ret1:
    result = 1;
    return result;
ret0:
    result = 0;
    return result;
}
