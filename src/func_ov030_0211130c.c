// @symbol func_ov030_0211130c
// recovered name: daObjHmBskt_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov030_02115c80[];
/* recovered: renamed to Class_Method */
/* daObjHmBskt_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov030_02115c88[];
int func_ov030_0211130c(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov030_02115c88);
    _ZN13SharedFilePtr7ReleaseEv(data_ov030_02115c80);
    return 1;
}
