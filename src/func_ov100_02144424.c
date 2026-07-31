// @symbol func_ov100_02144424
// @emits daDoor_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daDoor_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *G0;
int *daDoor_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)VT0;
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
