// @symbol _ZN5SwoopD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV12daBasabasa_c */
extern void func_ov002_020aed18(void *);
extern void *data_020a0eac;
int *_ZN5SwoopD0Ev(int *t)
{
    t[0] = (int)_ZTV12daBasabasa_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x3c8);
    _ZN9ModelAnimD1Ev((char *)t + 0x364);
    _ZN9ModelAnimD1Ev((char *)t + 0x300);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x144);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
