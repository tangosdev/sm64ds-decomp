// @symbol _ZN7HeaveHoD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV9daPopoi_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern void *data_020a0eac;
int *_ZN7HeaveHoD0Ev(int *t)
{
    t[0] = (int)_ZTV9daPopoi_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x3a4);
    _ZN9ModelAnimD1Ev((char *)t + 0x340);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x184);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
