// @symbol _ZN5SwoopD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV5Swoop */
extern void _ZN5EnemyD2Ev(void *);
int *_ZN5SwoopD1Ev(int *t)
{
    t[0] = (int)_ZTV5Swoop;
    _ZN11ShadowModelD1Ev((char *)t + 0x3c8);
    _ZN9ModelAnimD1Ev((char *)t + 0x364);
    _ZN9ModelAnimD1Ev((char *)t + 0x300);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
