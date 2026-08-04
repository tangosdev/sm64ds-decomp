// @symbol func_ov043_0211123c
// recovered name: daObjKm1_Ukishima_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov043_021125e0[];
/* recovered: renamed to Class_Method */
/* daObjKm1_Ukishima_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov043_021125e8[];
int func_ov043_0211123c(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov043_021125e8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov043_021125e0);
    return 1;
}
