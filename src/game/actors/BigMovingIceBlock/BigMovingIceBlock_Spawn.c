// @symbol BigMovingIceBlock_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV17BigMovingIceBlock */
extern void _ZN7PathPtrC1Ev(void *);
int *BigMovingIceBlock_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(816);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV17BigMovingIceBlock;
        _ZN7PathPtrC1Ev((char *)p + 0x320);
    }
    return p;
}
