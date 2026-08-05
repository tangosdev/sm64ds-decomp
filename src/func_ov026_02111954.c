// @symbol func_ov026_02111954
// recovered name: daObjWlSubmarine_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int Submarine_ClsnFile[];
/* recovered: renamed to Class_Method */
/* daObjWlSubmarine_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int Submarine_ModelFile[];
int func_ov026_02111954(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(Submarine_ModelFile);
    _ZN13SharedFilePtr7ReleaseEv(Submarine_ClsnFile);
    return 1;
}
