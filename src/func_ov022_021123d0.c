// @symbol func_ov022_021123d0
// @emits daObjFl_Fall_Block_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
extern void *data_020a0eac;
extern int _ZTV16daObjFallBlock_c[];
extern int _ZTV10dBgActor_c[];
extern int data_ov022_021142c4[];
int *daObjFl_Fall_Block_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)data_ov022_021142c4;
    t[0] = (int)_ZTV16daObjFallBlock_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
