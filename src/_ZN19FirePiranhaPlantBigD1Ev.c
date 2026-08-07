// @symbol _ZN19FirePiranhaPlantBigD1Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV19FirePiranhaPlantBig */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
int *_ZN19FirePiranhaPlantBigD1Ev(int *t)
{
    t[0] = (int)_ZTV19FirePiranhaPlantBig;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x1a8);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    return t;
}
