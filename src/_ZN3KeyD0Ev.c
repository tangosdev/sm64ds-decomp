// @symbol _ZN3KeyD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV10daObjKey_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN5EnemyD2Ev(void *);
extern void *data_020a0eac;
int *_ZN3KeyD0Ev(int *t)
{
    t[0] = (int)_ZTV10daObjKey_c;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x260);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x220);
    _ZN11ShadowModelD1Ev((char *)t + 0x1c8);
    _ZN5ModelD1Ev((char *)t + 0x178);
    _ZN9ModelAnimD1Ev((char *)t + 0x114);
    _ZN5EnemyD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
