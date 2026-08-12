// @symbol func_ov012_021112ec
// recovered name: daObjC0_Switch_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjC0_Switch_c::CleanupResources - verified 2004/b56 byte-match (ov012), strict-reloc */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov012_021124a8[];
int func_ov012_021112ec(void *t)
{
    _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov012_021124a8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov012_021124a0);
    return 1;
}
