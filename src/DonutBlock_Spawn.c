// @symbol DonutBlock_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10DonutBlock */
int *DonutBlock_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1260);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV10DonutBlock;
        _ZN12WithMeshClsnC1Ev((char *)p + 0x320);
    }
    return p;
}
