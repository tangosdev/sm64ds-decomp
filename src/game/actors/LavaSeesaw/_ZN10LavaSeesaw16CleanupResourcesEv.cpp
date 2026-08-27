//cpp
// @symbol _ZN10LavaSeesaw16CleanupResourcesEv
#include "LavaSeesaw.h"
// recovered name: daObjFl_Seesaw_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov022_021145a0[];
/* recovered: renamed to Class_Method */
/* daObjFl_Seesaw_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov022_021145a8[];
}

s32 LavaSeesaw::CleanupResources() {
    void * t = (void *)this;
    if (_ZN4dBgW9IsEnabledEv((char *)t + 0x124)) {
        _ZN4dBgW7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_021145a8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_021145a0);
    return 1;
}
