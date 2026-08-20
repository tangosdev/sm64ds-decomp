//cpp
// @symbol _ZN12SwitchPillar16CleanupResourcesEv
#include "SwitchPillar.h"
// recovered name: daObjC0_Switch_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjC0_Switch_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov012_021124a8[];
}

s32 SwitchPillar::CleanupResources() {
    void * t = (void *)this;
    _ZN4dBgW7DisableEv((char *)t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov012_021124a8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov012_021124a0);
    return 1;
}
