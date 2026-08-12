//cpp
// @symbol _ZN15FireSeaElevator16CleanupResourcesEv
#include "FireSeaElevator.h"
// recovered name: daObjKm2_Agaru_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjKm2_Agaru_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov045_02113188[];
}

s32 FireSeaElevator::CleanupResources() {
    void * t = (void *)this;
    _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov045_02113188);
    _ZN13SharedFilePtr7ReleaseEv(data_ov045_02113180);
    return 1;
}
