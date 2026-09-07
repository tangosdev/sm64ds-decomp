//cpp
// @symbol _ZN11dScMiniGm_c16CleanupResourcesEv
/* recovered: real C++ method */
/* dScMiniGm_c::CleanupResources() -- vtable slot 3. Tears the running minigame
 * down only if one was actually selected (data_0208a174[0] >= 0), then drops
 * the voice group. */
#include "dScMiniGm_c.h"
#include "decl_common.h"

extern "C" void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);

s32 dScMiniGm_c::CleanupResources()
{
    if (data_0208a174[0] >= 0) {
        func_ov005_020c0030((int)this);
    }
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    return 1;
}
