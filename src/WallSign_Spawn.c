// @symbol WallSign_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8WallSign */
int *WallSign_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(872);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV8WallSign;
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x320);
    }
    return p;
}
