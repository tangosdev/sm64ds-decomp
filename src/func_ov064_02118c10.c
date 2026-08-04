// @symbol func_ov064_02118c10
// recovered name: daObjFl_Coin_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
extern void* data_020a0eac;
extern int data_ov064_0211c1d8[];
/* recovered: renamed to Class_Method */
/* daObjFl_Coin_c::OnYoshiTryEat - recovered from vtable slot identity */
int *func_ov064_02118c10(int *t)
{
    t[0] = (int)data_ov064_0211c1d8;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
