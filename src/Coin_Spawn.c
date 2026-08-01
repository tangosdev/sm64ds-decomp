// @symbol Coin_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV4Coin */
int *Coin_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(948);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV4Coin;
        _ZN11CommonModelC1Ev((char *)p + 0xd8);
        _ZN11CommonModelC1Ev((char *)p + 0x114);
        _ZN11ShadowModelC1Ev((char *)p + 0x150);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x178);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1ac);
    }
    return p;
}
