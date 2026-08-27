//cpp
// @symbol _ZN12MetalNetLift16CleanupResourcesEv
#include "MetalNetLift.h"
// recovered name: daObjFl_Amilift_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov064_0211c728[];
/* recovered: renamed to Class_Method */
/* daObjFl_Amilift_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov064_0211c730[];
}

s32 MetalNetLift::CleanupResources() {
    void * t = (void *)this;
    _ZN4dBgW7DisableEv((char *)t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c730);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c728);
    return 1;
}
