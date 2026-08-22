//cpp
// @symbol _ZN10dScTitle_c16CleanupResourcesEv
/* recovered: real C++ method */
/* dScTitle_c::CleanupResources() -- vtable slot 3. The title scene owns only
 * its voice group; releasing that is the whole teardown. */
#include "dScTitle_c.h"

extern "C" void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);

s32 dScTitle_c::CleanupResources()
{
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    return 1;
}
