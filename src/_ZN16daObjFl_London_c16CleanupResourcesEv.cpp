//cpp
// @symbol _ZN16daObjFl_London_c16CleanupResourcesEv
#include "daObjFl_London_c.h"
// recovered name: daObjFl_London_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov022_02114578[];
/* recovered: renamed to Class_Method */
/* daObjFl_London_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov022_02114580[];
}

s32 daObjFl_London_c::CleanupResources() {
    void * t = (void *)this;
    if (_ZN4dBgW9IsEnabledEv((char *)t + 0x124)) {
        _ZN4dBgW7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_02114580);
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_02114578);
    return 1;
}
