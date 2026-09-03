//cpp
// @symbol _ZN16daObjKm2_Agaru_c16CleanupResourcesEv
#include "daObjKm2_Agaru_c.h"
// recovered name: daObjKm2_Agaru_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjKm2_Agaru_c::CleanupResources - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov045_02113188[];
}

s32 daObjKm2_Agaru_c::CleanupResources() {
    void * t = (void *)this;
    _ZN4dBgW7DisableEv((char *)t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov045_02113188);
    _ZN13SharedFilePtr7ReleaseEv(data_ov045_02113180);
    return 1;
}
