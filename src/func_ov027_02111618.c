// @symbol func_ov027_02111618
// @emits daIDonketu_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daIDonketu_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void func_ov002_020aed18(void *);
extern void *G0;
int *daIDonketu_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)VT0;
    t[0] = (int)VT1;
    _ZN11ShadowModelD1Ev((char *)t + 0x370);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x33c);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x174);
    _ZN9ModelAnimD1Ev((char *)t + 0x110);
    func_ov002_020aed18(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
