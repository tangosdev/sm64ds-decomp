// @symbol func_ov065_0211a15c
// recovered name: daObjCtMecha03_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
extern int data_ov065_0211d894[];
/* recovered: renamed to Class_Method */
/* daObjCtMecha03_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov065_0211d88c[];
int func_ov065_0211a15c(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d88c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov065_0211d894);
    return 1;
}
