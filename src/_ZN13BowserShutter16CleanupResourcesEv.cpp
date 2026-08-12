//cpp
// @symbol _ZN13BowserShutter16CleanupResourcesEv
#include "BowserShutter.h"
// recovered name: daObjWlKoopaShutter_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int BowserShutter_ClsnFile[];
/* recovered: renamed to Class_Method */
/* daObjWlKoopaShutter_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int BowserShutter_ModelFile[];
}

s32 BowserShutter::CleanupResources() {
    void * t = (void *)this;
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(BowserShutter_ModelFile);
    _ZN13SharedFilePtr7ReleaseEv(BowserShutter_ClsnFile);
    return 1;
}
