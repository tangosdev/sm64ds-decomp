// @symbol _ZN14KnockDownPlankD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV19daObjBk_Dossunbar_c; VT1 = _ZTV10dBgActor_c */
extern void *G0;
int *_ZN14KnockDownPlankD0Ev(int *t)
{
    t[0] = (int)_ZTV19daObjBk_Dossunbar_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
