// @symbol UkikiCage_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
extern int data_ov030_02115974[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov030_02115974 */
int *UkikiCage_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1248);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov030_02115974;
        _ZN12WithMeshClsnC1Ev((char *)p + 0x320);
    }
    return p;
}
