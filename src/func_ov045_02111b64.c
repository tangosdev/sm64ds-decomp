// @symbol func_ov045_02111b64
// recovered name: daObjKm2_Ukishima_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daObjKm2_Ukishima_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *G0;
int *func_ov045_02111b64(int *t)
{
    t[0] = (int)VT0;
    t[0] = (int)VT1;
    t[0] = (int)VT2;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
