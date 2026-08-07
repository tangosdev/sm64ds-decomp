// @symbol _ZN19FirePiranhaPlantBigD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV8daFPkn_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN19FirePiranhaPlantBigD0Ev(int *t)
{
    t[0] = (int)_ZTV8daFPkn_c;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x1a8);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
