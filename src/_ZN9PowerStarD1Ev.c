// @symbol _ZN9PowerStarD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9PowerStar */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
int *_ZN9PowerStarD1Ev(int *t)
{
    t[0] = (int)_ZTV9PowerStar;
    _ZN11ShadowModelD1Ev((char *)t + 0x3d4);
    _ZN9ModelAnimD1Ev((char *)t + 0x370);
    _ZN9ModelAnimD1Ev((char *)t + 0x30c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
