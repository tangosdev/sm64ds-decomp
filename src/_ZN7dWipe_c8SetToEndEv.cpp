//cpp
// @symbol _ZN7dWipe_c8SetToEndEv
/* recovered: real C++ method */
/* dWipe_c::SetToEnd() -- vtable slot 8. Same shape as SetToStart: the ROM
 * emits the interworking veneer to FaderBrightness::SetToEnd (0x0201761c),
 * not a body. */
#include "dWipe_c.h"

void dWipe_c::SetToEnd()
{
    FaderBrightness::SetToEnd();
}
