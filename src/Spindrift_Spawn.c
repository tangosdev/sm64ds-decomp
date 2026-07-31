// @symbol Spindrift_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9Spindrift */
int *Spindrift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(924);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV9Spindrift;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN11ShadowModelC1Ev((char *)p + 0x174);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x19c);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x1d0);
    }
    return p;
}
