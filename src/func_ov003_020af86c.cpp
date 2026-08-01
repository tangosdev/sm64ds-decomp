//cpp
// @symbol func_ov003_020af86c
// recovered name: dScStarSel_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScStarSel_c::CleanupResources - recovered from vtable slot identity */
class Sound {
public:
    static void UnsetPlayerVoiceGroup();
};


extern "C" int func_ov003_020af86c(void) {
    CleanCommonModelDataArr();
    Sound::UnsetPlayerVoiceGroup();
    unsigned short *p = (unsigned short *)0x4000304;
    *p = *p | 0x8000;
    return 1;
}
