// @symbol _ZN10BulletBillD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV7daKlr_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN10BulletBillD0Ev(int *t)
{
    t[0] = (int)_ZTV7daKlr_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x3ac);
    _ZN5ModelD1Ev((char *)t + 0x35c);
    _ZN5ModelD1Ev((char *)t + 0x30c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
