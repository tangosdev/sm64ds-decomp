// @symbol _ZN10dScEntry_c16CleanupResourcesEv
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* dScEntry_c::CleanupResources() -- vtable slot 3. Plain C function
 * carrying the literal mangled name (implicit `this`, spelled here as
 * `levelID` since only its low byte is live -- see include/dScEntry_c.h). */
extern void func_020308b4(signed char levelID);
extern void *data_0209d4a8;
extern void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
int _ZN10dScEntry_c16CleanupResourcesEv(signed char levelID)
{
    func_020308b4(levelID);
    if (data_0209b2e8 != (void *)0) {
        Deallocate(data_0209b2e8);
        data_0209b2e8 = (void *)0;
    }
    data_0209d4a8 = (void *)0;
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    CleanCommonModelDataArr();
    return 1;
}
