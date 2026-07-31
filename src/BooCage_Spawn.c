// @symbol BooCage_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV7BooCage */
int *BooCage_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(896);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV7BooCage;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x110);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x144);
        _ZN5ModelC1Ev((char *)p + 0x300);
        _ZN11ShadowModelC1Ev((char *)p + 0x350);
    }
    return p;
}
