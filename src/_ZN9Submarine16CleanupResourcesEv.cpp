//cpp
// @symbol _ZN9Submarine16CleanupResourcesEv
#include "Submarine.h"
// recovered name: daObjWlSubmarine_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int Submarine_ClsnFile[];
/* recovered: renamed to Class_Method */
/* daObjWlSubmarine_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int Submarine_ModelFile[];
}

s32 Submarine::CleanupResources() {
    void * t = (void *)this;
    if (_ZN4dBgW9IsEnabledEv((char *)t + 0x124)) {
        _ZN4dBgW7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(Submarine_ModelFile);
    _ZN13SharedFilePtr7ReleaseEv(Submarine_ClsnFile);
    return 1;
}
