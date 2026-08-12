// @symbol func_ov018_021112fc
// recovered name: daObjSm_Lift_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjSm_Lift_c::CleanupResources - verified 2004/b56 byte-match (ov018), strict-reloc */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov018_02113bc8[];
int func_ov018_021112fc(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov018_02113bc8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov018_02113bc0);
    return 1;
}
