//cpp
// @symbol _ZN7dWipe_c10SetToStartEv
/* recovered: real C++ method */
/* dWipe_c::SetToStart() -- vtable slot 9. A pure veneer to the base: the wipe
 * has no start state of its own. The qualified call is what keeps it a direct
 * `bl` instead of a virtual dispatch back into this same slot. */
#include "dWipe_c.h"

void dWipe_c::SetToStart()
{
    FaderBrightness::SetToStart();
}
