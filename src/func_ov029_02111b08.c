// @symbol func_ov029_02111b08
// @emits daObjWc_Obj05_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daObjWc_Obj05_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *G0;
int *daObjWc_Obj05_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)VT0;
    t[0] = (int)VT1;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
