// @symbol func_ov036_021111e4
// @emits daObjRcBuranko_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daObjRcBuranko_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *G0;
int *daObjRcBuranko_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)VT0;
    t[0] = (int)VT1;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
