// @symbol GoombaLarge_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_CapEnemy.h"
#include "decl_MaterialChanger.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV6Goomba */
int *GoombaLarge_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1144);
    if (p) {
        _ZN8CapEnemyC2Ev(p);
        p[0] = (int)_ZTV6Goomba;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x180);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1b4);
        _ZN9ModelAnimC1Ev((char *)p + 0x370);
        _ZN11ShadowModelC1Ev((char *)p + 0x3d4);
        _ZN15MaterialChangerC1Ev((char *)p + 0x3fc);
    }
    return p;
}
