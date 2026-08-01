// @symbol KingBobOmb_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_BlendModelAnim.h"
#include "decl_Enemy.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10KingBobOmb */
int *KingBobOmb_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1292);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV10KingBobOmb;
        _ZN12WithMeshClsnC1Ev((char *)p + 0x110);
        _ZN14BlendModelAnimC1Ev((char *)p + 0x2cc);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x33c);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x37c);
        _ZN11CommonModelC1Ev((char *)p + 0x3bc);
        _ZN11ShadowModelC1Ev((char *)p + 0x3f8);
    }
    return p;
}
