// @symbol func_ov002_020bc444
// recovered name: daObjWakame_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daObjWakame_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *G0;
int *func_ov002_020bc444(int *t)
{
    t[0] = (int)VT0;
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
