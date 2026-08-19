// @symbol _ZN6GoombaD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV7daKrb_c */
extern void _ZN15MaterialChangerD1Ev(void *);
extern void func_ov002_020aedbc(void *);
extern void *data_020a0eac;
int *_ZN6GoombaD0Ev(int *t)
{
    t[0] = (int)_ZTV7daKrb_c;
    _ZN15MaterialChangerD1Ev((char *)t + 0x3fc);
    _ZN11ShadowModelD1Ev((char *)t + 0x3d4);
    _ZN9ModelAnimD1Ev((char *)t + 0x370);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1b4);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x180);
    func_ov002_020aedbc(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
