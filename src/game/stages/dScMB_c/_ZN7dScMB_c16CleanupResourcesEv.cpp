//cpp
// @symbol _ZN7dScMB_c16CleanupResourcesEv
/* recovered: real C++ method */
/* dScMB_c::CleanupResources() -- vtable slot 3. Unregisters the global
 * graphCallback_c object InitResources installed at data_0209d4a8, then hands
 * the colour fader back to dScene_c. Touches none of its own fields. */
#include "dScMB_c.h"

extern "C" void *data_0209d4a8; /* 0x0209d4a8 -- the live graph callback */

s32 dScMB_c::CleanupResources()
{
    data_0209d4a8 = 0;
    dScene_c::SetAndStopColorFader();
    return 1;
}
