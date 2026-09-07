//cpp
// @symbol _ZN7dWipe_c7IsAtEndEv
/* recovered: named members + real C++ method */
/* dWipe_c::IsAtEnd() -- vtable slot 6. Mirror of IsAtStart: type 1 defers to
 * the base, an idle state machine counts as finished, and otherwise the wipe
 * is done once its own ramp has run back down to zero. */
#include "dWipe_c.h"

int dWipe_c::IsAtEnd()
{
    int result;
    if (type == 1)
        return FaderBrightness::IsAtEnd();
    if (state == 0)
        goto ret1;
    if (wipeInterp > 0)
        goto ret0;
ret1:
    result = 1;
    return result;
ret0:
    result = 0;
    return result;
}
