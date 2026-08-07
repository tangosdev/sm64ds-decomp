// @symbol _ZN8BigBullyD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
extern int _ZTV8BigBully[];
extern int data_ov064_0211b768[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV8BigBully */
extern void _ZN5EnemyD2Ev(void *);
int *_ZN8BigBullyD1Ev(int *t)
{
    t[0] = (int)_ZTV8BigBully;
    t[0] = (int)data_ov064_0211b768;
    _ZN11ShadowModelD1Ev((char *)t + 0x370);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x33c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
