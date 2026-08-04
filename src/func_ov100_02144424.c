// @symbol func_ov100_02144424
// recovered name: daDoor_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int data_ov100_02148188[];
/* recovered: vtable identified, renamed to Class_Method */
/* daDoor_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *data_020a0eac;
int *func_ov100_02144424(int *t)
{
    t[0] = (int)data_ov100_02148188;
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
