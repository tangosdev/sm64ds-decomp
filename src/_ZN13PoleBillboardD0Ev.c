// @symbol func_ov015_021111d0
// recovered name: daObjBkBillboard_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
extern int data_ov015_02114360[];
/* recovered: vtable identified, renamed to Class_Method */
/* daObjBkBillboard_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *data_020a0eac;
int *func_ov015_021111d0(int *t)
{
    t[0] = (int)data_ov015_02114360;
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
