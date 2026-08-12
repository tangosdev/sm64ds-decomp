//cpp
// @symbol _ZN16SwingingPlatform16CleanupResourcesEv
#include "SwingingPlatform.h"
// recovered name: daObjRcBuranko_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov036_02114020[];
/* recovered: renamed to Class_Method */
/* daObjRcBuranko_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov036_02114028[];
}

s32 SwingingPlatform::CleanupResources() {
    void * t = (void *)this;
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov036_02114028);
    _ZN13SharedFilePtr7ReleaseEv(data_ov036_02114020);
    return 1;
}
