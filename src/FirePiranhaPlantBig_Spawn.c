// @symbol FirePiranhaPlantBig_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV19FirePiranhaPlantBig */
int *FirePiranhaPlantBig_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(556);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV19FirePiranhaPlantBig;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x174);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x1a8);
    }
    return p;
}
