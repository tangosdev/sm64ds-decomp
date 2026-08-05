// @symbol _ZN5BullyD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
extern int _ZTV5Bully[];
extern int data_ov064_0211b768[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV5Bully */
extern void func_ov002_020aed18(void *);
extern void *data_020a0eac;
int *_ZN5BullyD0Ev(int *t)
{
    t[0] = (int)_ZTV5Bully;
    t[0] = (int)data_ov064_0211b768;
    _ZN11ShadowModelD1Ev((char *)t + 0x370);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x33c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
