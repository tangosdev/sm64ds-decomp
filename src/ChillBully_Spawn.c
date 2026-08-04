// @symbol ChillBully_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
extern int data_ov064_0211b768[];
extern int data_ov027_02113930[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov064_0211b768 */
int *ChillBully_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(1020);
    if (p) {
        _ZN5EnemyC2Ev(p);
        p[0] = (int)data_ov064_0211b768;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN12WithMeshClsnC1Ev((char *)p + 0x174);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x33c);
        _ZN11ShadowModelC1Ev((char *)p + 0x370);
        p[0] = (int)data_ov027_02113930;
    }
    return p;
}
