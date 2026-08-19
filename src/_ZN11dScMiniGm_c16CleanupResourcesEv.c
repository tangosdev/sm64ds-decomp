// @symbol _ZN11dScMiniGm_c16CleanupResourcesEv
/* dScMiniGm_c::CleanupResources() -- vtable slot 3. See include/dScMiniGm_c.h. */
#include "decl_common.h"
extern void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
int _ZN11dScMiniGm_c16CleanupResourcesEv(int c)
{
    if (data_0208a174[0] >= 0) {
        func_ov005_020c0030(c);
    }
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    return 1;
}
