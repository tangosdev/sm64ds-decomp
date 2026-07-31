// @symbol _ZN10KingBobOmbD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_BlendModelAnim.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV12daBombking_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void func_ov002_020aed18(void *);
extern void *G0;
int *_ZN10KingBobOmbD0Ev(int *t)
{
    t[0] = (int)_ZTV12daBombking_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x3f8);
    _ZN11CommonModelD1Ev((char *)t + 0x3bc);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x37c);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x33c);
    _ZN14BlendModelAnimD1Ev((char *)t + 0x2cc);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
