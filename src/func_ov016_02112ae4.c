// @symbol func_ov016_02112ae4
// recovered name: daObjKi_Hasira_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int RockPillar_ClsnFile[];
/* recovered: renamed to Class_Method */
/* daObjKi_Hasira_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int RockPillar_ModelFile[];
int func_ov016_02112ae4(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(RockPillar_ModelFile);
    _ZN13SharedFilePtr7ReleaseEv(RockPillar_ClsnFile);
    return 1;
}
