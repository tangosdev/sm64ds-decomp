// @symbol func_ov064_02117cc4
// @emits daObjFl_Amilift_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjFl_Amilift_c::CleanupResources - recovered from vtable slot identity */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int G0[];
int daObjFl_Amilift_c_CleanupResources(void *t)
{
    _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(G0);
    _ZN13SharedFilePtr7ReleaseEv(G1);
    return 1;
}
