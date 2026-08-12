// @symbol UkikiCage_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
extern int _ZTV9UkikiCage[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV9UkikiCage */
int *UkikiCage_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1248);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV9UkikiCage;
        _ZN12WithMeshClsnC1Ev((char *)p + 0x320);
    }
    return p;
}
