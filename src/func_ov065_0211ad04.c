// @symbol func_ov065_0211ad04
// recovered name: daObjCtMecha05_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int TTC_MovingBar_ClsnFile[];
/* recovered: renamed to Class_Method */
/* daObjCtMecha05_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int TTC_MovingBar_ModelFile[];
int func_ov065_0211ad04(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(TTC_MovingBar_ModelFile);
    _ZN13SharedFilePtr7ReleaseEv(TTC_MovingBar_ClsnFile);
    return 1;
}
