// @symbol Key_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsnWithPos.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV3Key */
int *Key_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1136);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)_ZTV3Key;
        _ZN9ModelAnimC1Ev((char *)p + 0x114);
        _ZN5ModelC1Ev((char *)p + 0x178);
        _ZN11ShadowModelC1Ev((char *)p + 0x1c8);
        _ZN25MovingCylinderClsnWithPosC1Ev((char *)p + 0x220);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x260);
    }
    return p;
}
