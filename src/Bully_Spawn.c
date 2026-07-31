// @symbol Bully_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV11daDonketu_c */
int *Bully_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1024);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV11daDonketu_c;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x174);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x33c);
        _ZN11ShadowModelC1Ev((char *)p + 0x370);
        p[0] = (int)VT1;
    }
    return p;
}
