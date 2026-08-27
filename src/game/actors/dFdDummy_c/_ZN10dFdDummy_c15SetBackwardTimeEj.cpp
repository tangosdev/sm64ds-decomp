//cpp
// @symbol _ZN10dFdDummy_c15SetBackwardTimeEj
/* dFdDummy_c::SetBackwardTime -- recovered from vtable slot identity.
 * Writes speed = -0x1000, then makes a genuine virtual call through slot 5
 * (dFdDummy_c overrides none of slots 5-9, so this lands on
 * FaderColor/FaderBrightness's own inherited body). */
#include "dFdDummy_c.h"

int dFdDummy_c::SetBackwardTime(u32)
{
    speed = -0x1000;
    return IsAtStart();
}
