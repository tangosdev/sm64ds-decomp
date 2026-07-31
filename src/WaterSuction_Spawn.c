// @symbol WaterSuction_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12WaterSuction */
int *WaterSuction_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(792);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV12WaterSuction;
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x110);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x150);
    }
    return p;
}
