// @symbol MrI_Projectile_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV14MrI_Projectile */
int *MrI_Projectile_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(820);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV14MrI_Projectile;
        _ZN11ShadowModelC1Ev((char *)p + 0xd4);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0xfc);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x13c);
    }
    return p;
}
