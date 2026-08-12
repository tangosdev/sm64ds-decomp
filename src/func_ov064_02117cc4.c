// @symbol func_ov064_02117cc4
// recovered name: daObjFl_Amilift_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjFl_Amilift_c::CleanupResources - verified 2004/b56 byte-match (ov064), strict-reloc */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov064_0211c728[];
extern int data_ov064_0211c730[];
int func_ov064_02117cc4(void *t)
{
    _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c730);
    _ZN13SharedFilePtr7ReleaseEv(data_ov064_0211c728);
    return 1;
}
