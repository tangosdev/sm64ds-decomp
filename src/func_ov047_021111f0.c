// @symbol func_ov047_021111f0
// recovered name: daObjKm3_Kurumajiku_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int data_ov047_021122a0[];
extern int data_ov002_02109320[];
extern int _ZTV8Platform[];
/* recovered: vtable identified, renamed to Class_Method */
/* daObjKm3_Kurumajiku_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void *data_020a0eac;
int *func_ov047_021111f0(int *t)
{
    t[0] = (int)data_ov047_021122a0;
    t[0] = (int)data_ov002_02109320;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
