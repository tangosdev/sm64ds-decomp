//cpp
// @symbol _ZN7SkiLift16CleanupResourcesEv
#include "SkiLift.h"
// recovered name: daObjSm_Lift_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjSm_Lift_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov018_02113bc8[];
}

s32 SkiLift::CleanupResources() {
    void * t = (void *)this;
    if (_ZN4dBgW9IsEnabledEv((char *)t + 0x124)) {
        _ZN4dBgW7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov018_02113bc8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov018_02113bc0);
    return 1;
}
