// @symbol PushBlock_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
extern int data_ov002_021096b0[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_021096b0 */
int *PushBlock_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1268);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov002_021096b0;
        _ZN12WithMeshClsnC1Ev((char *)p + 0x320);
    }
    return p;
}
