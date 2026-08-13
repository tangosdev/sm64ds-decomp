//cpp
// @symbol _ZN11DiamondLift16CleanupResourcesEv
#include "DiamondLift.h"
// recovered name: daObjKm1_Ukishima_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov043_021125e0[];
/* recovered: renamed to Class_Method */
/* daObjKm1_Ukishima_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov043_021125e8[];
}

s32 DiamondLift::CleanupResources() {
    void * t = (void *)this;
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov043_021125e8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov043_021125e0);
    return 1;
}
