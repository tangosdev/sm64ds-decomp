// @symbol _ZN6RabbitD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV7daMip_c */
extern void func_ov002_020aed18(void *);
extern void *G0;
int *_ZN6RabbitD0Ev(int *t)
{
    t[0] = (int)_ZTV7daMip_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x3c0);
    _ZN11ShadowModelD1Ev((char *)t + 0x368);
    _ZN9ModelAnimD1Ev((char *)t + 0x300);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
