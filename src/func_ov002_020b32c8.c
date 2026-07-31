// @symbol func_ov002_020b32c8
// @emits daObjAbuku_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daObjAbuku_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *G0;
int *daObjAbuku_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)VT0;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
