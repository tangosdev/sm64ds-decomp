// @symbol func_ov002_020b6d4c
// recovered name: daObjLava_c_OnYoshiTryEat
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
extern void* data_020a0eac;
extern int data_ov002_021093e0[];
/* recovered: renamed to Class_Method */
/* daObjLava_c::OnYoshiTryEat - recovered from vtable slot identity */
int *func_ov002_020b6d4c(int *t)
{
    t[0] = (int)data_ov002_021093e0;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
