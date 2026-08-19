//cpp
// @symbol _ZN5Stump16CleanupResourcesEv
#include "Stump.h"
// recovered name: daObjPile_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjPile_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov091_02135654[];
}

s32 Stump::CleanupResources() {
    void * t = (void *)this;
    if (_ZN4dBgW9IsEnabledEv((char *)t + 0x124)) {
        _ZN4dBgW7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov091_02135654);
    _ZN13SharedFilePtr7ReleaseEv(data_ov091_0213564c);
    return 1;
}
