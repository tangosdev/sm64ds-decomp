//cpp
#include "Sound.h"
// @symbol _ZN10dScTitle_c16CleanupResourcesEv
/* recovered: real C++ method */
/* dScTitle_c::CleanupResources() -- vtable slot 3. The title scene owns only
 * its voice group; releasing that is the whole teardown. */
#include "dScTitle_c.h"


s32 dScTitle_c::CleanupResources()
{
    Sound::UnsetPlayerVoiceGroup();
    return 1;
}
