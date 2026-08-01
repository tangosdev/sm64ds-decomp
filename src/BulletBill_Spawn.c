// @symbol BulletBill_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10BulletBill */
int *BulletBill_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(992);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV10BulletBill;
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x110);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x150);
        _ZN5ModelC1Ev((char *)p + 0x30c);
        _ZN5ModelC1Ev((char *)p + 0x35c);
        _ZN11ShadowModelC1Ev((char *)p + 0x3ac);
    }
    return p;
}
