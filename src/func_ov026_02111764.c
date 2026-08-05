// @symbol func_ov026_02111764
// recovered name: daObjWlKoopaShutter_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int BowserShutter_ClsnFile[];
/* recovered: renamed to Class_Method */
/* daObjWlKoopaShutter_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int BowserShutter_ModelFile[];
int func_ov026_02111764(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(BowserShutter_ModelFile);
    _ZN13SharedFilePtr7ReleaseEv(BowserShutter_ClsnFile);
    return 1;
}
