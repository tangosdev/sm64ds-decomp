// @symbol _ZN7ChuckyaD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV10daHolhei_c */
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN7ChuckyaD0Ev(int *t)
{
    t[0] = (int)_ZTV10daHolhei_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x368);
    _ZN9ModelAnimD1Ev((char *)t + 0x300);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
