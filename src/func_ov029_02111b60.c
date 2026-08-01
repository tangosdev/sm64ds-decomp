// @symbol func_ov029_02111b60
// recovered name: daObjWc_Obj05_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjWc_Obj05_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov029_0211428c[];
int func_ov029_02111b60(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov029_0211428c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov029_02114284);
    return 1;
}
