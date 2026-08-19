//cpp
// @symbol _ZN10RockPillar16CleanupResourcesEv
#include "RockPillar.h"
// recovered name: daObjKi_Hasira_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int RockPillar_ClsnFile[];
/* recovered: renamed to Class_Method */
/* daObjKi_Hasira_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int RockPillar_ModelFile[];
}

s32 RockPillar::CleanupResources() {
    void * t = (void *)this;
    if (_ZN4dBgW9IsEnabledEv((char *)t + 0x124)) {
        _ZN4dBgW7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(RockPillar_ModelFile);
    _ZN13SharedFilePtr7ReleaseEv(RockPillar_ClsnFile);
    return 1;
}
