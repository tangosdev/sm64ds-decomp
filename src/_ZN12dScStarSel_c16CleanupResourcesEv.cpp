//cpp
// @symbol _ZN12dScStarSel_c16CleanupResourcesEv
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* dScStarSel_c::CleanupResources() -- vtable slot 3. extern "C" carries the
 * literal mangled name unmangled -- see include/dScStarSel_c.h. */
class Sound {
public:
    static void UnsetPlayerVoiceGroup();
};


extern "C" int _ZN12dScStarSel_c16CleanupResourcesEv(void) {
    CleanCommonModelDataArr();
    Sound::UnsetPlayerVoiceGroup();
    unsigned short *p = (unsigned short *)0x4000304;
    *p = *p | 0x8000;
    return 1;
}
