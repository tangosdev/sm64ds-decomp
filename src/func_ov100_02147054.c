// @symbol func_ov100_02147054
// recovered name: daObjPathLift_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjPathLift_c::CleanupResources - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
/* Three SharedFilePtrs, and the first one lives in a different overlay: the retail
   relocation for that site resolves into ov002, the other two into ov100. They were
   all spelled G0/G1/G2 here, which hid the crossing. decl_common.h already declares
   the latter two by their real names (as scalars, hence the address-of below); it
   also still declares G1[] and G2[], which nothing defines. */
extern int data_ov002_0210d9f0[];
int func_ov100_02147054(void *t)
{
    if (_ZN16MeshColliderBase9IsEnabledEv((char *)t + 0x124)) {
        _ZN16MeshColliderBase7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9f0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov100_02148a54);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov100_02148a5c);
    return 1;
}
