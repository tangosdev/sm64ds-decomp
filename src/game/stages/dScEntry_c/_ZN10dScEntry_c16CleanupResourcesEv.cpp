//cpp
// @symbol _ZN10dScEntry_c16CleanupResourcesEv
/* recovered: real C++ method */
/* dScEntry_c::CleanupResources() -- vtable slot 3. Releases the level-entry
 * scratch buffer, unregisters the graph callback, drops the voice group and
 * clears the shared model-data array. */
#include "dScEntry_c.h"
#include "decl_common.h"

extern "C" {
void func_020308b4(void *self);
void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
extern void *data_0209d4a8;
}

s32 dScEntry_c::CleanupResources()
{
    func_020308b4(this);
    if (data_0209b2e8 != (void *)0) {
        Deallocate(data_0209b2e8);
        data_0209b2e8 = (void *)0;
    }
    data_0209d4a8 = (void *)0;
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    CleanCommonModelDataArr();
    return 1;
}
