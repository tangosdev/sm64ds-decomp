//cpp
// @symbol _ZN9TinyCover16CleanupResourcesEv
#include "TinyCover.h"
// recovered name: daObjTtFuta_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjTtFuta_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov033_021124c8[];
}

s32 TinyCover::CleanupResources() {
    void * t = (void *)this;
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov033_021124c8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov033_021124c0);
    return 1;
}
