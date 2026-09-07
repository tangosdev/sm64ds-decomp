//cpp
// @symbol _ZN12dScStarSel_c16CleanupResourcesEv
/* dScStarSel_c::CleanupResources() -- vtable slot 3. Real C++ method over the
 * shared header. */
#include "decl_common.h"
#include "dScStarSel_c.h"
class Sound {
public:
    static void UnsetPlayerVoiceGroup();
};

s32 dScStarSel_c::CleanupResources() {
    CleanCommonModelDataArr();
    Sound::UnsetPlayerVoiceGroup();
    unsigned short *p = (unsigned short *)0x4000304;
    *p = *p | 0x8000;
    return 1;
}
