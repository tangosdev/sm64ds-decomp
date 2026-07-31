// @symbol func_ov002_020b6d4c
// @emits daObjLava_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daObjLava_c::OnYoshiTryEat - recovered from vtable slot identity */
int *daObjLava_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)VT;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, HEAP);
    return t;
}
