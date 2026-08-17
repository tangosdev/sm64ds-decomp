// @symbol _ZN12FallBlockBbhD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV10dBgActor_c[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV12FallBlockBbh; VT1 = _ZTV16daObjFallBlock_c */
extern void *data_020a0eac;
extern int _ZTV12FallBlockBbh[];
extern int _ZTV16daObjFallBlock_c[];
int *_ZN12FallBlockBbhD0Ev(int *t)
{
    t[0] = (int)_ZTV12FallBlockBbh;
    t[0] = (int)_ZTV16daObjFallBlock_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
