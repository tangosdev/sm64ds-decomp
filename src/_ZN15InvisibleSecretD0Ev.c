// @symbol func_ov002_020f03f4
// recovered name: daSCoin_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
extern int data_ov002_0210b030[];
/* recovered: vtable identified, renamed to Class_Method */
/* daSCoin_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *data_020a0eac;
int *func_ov002_020f03f4(int *t)
{
    t[0] = (int)data_ov002_0210b030;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
