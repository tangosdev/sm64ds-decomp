// @symbol func_ov032_02112544
// recovered name: daObjTdFuta_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjTdFuta_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov032_02113ad4[];
int func_ov032_02112544(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov032_02113ad4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov032_02113acc);
    return 1;
}
