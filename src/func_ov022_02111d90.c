// @symbol func_ov022_02111d90
// recovered name: daObjFl_Seesaw_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov022_021145a0[];
/* recovered: renamed to Class_Method */
/* daObjFl_Seesaw_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov022_021145a8[];
int func_ov022_02111d90(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_021145a8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov022_021145a0);
    return 1;
}
