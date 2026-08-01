// @symbol PiranhaPlant_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12PiranhaPlant */
int *PiranhaPlant_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1148);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV12PiranhaPlant;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN5ModelC1Ev((char *)p + 0x174);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1c4);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x380);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x3b4);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x3e8);
    }
    return p;
}
