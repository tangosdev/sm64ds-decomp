// @symbol func_ov002_020b8d68
// recovered name: daObjPushblock_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov002_0210df94[];
/* recovered: renamed to Class_Method */
/* daObjPushblock_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210df9c[];
int func_ov002_020b8d68(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210df9c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210df94);
    return 1;
}
