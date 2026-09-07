//cpp
// @symbol _ZN10dFdDummy_c14SetForwardTimeEj
/* dFdDummy_c::SetForwardTime -- recovered from vtable slot identity.
 * Writes speed = 0x1000, then makes a genuine virtual call through slot 6
 * (dFdDummy_c overrides none of slots 5-9, so this lands on
 * FaderColor/FaderBrightness's own inherited body). */
#include "dFdDummy_c.h"

int dFdDummy_c::SetForwardTime(u32)
{
    speed = 0x1000;
    return IsAtEnd();
}
