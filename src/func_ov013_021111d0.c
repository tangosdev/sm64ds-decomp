// @symbol func_ov013_021111d0
// recovered name: daObjClockHuriko_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
extern int data_ov013_02112128[];
/* recovered: vtable identified, renamed to Class_Method */
/* daObjClockHuriko_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *data_020a0eac;
int *func_ov013_021111d0(int *t)
{
    t[0] = (int)data_ov013_02112128;
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
