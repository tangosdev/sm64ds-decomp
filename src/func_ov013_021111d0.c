// @symbol func_ov013_021111d0
// recovered name: daObjClockHuriko_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daObjClockHuriko_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *G0;
int *func_ov013_021111d0(int *t)
{
    t[0] = (int)VT0;
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
