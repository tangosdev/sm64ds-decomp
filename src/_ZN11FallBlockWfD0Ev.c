// @symbol _ZN11FallBlockWfD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV11FallBlockWf */
extern void *data_020a0eac;
extern int _ZTV11FallBlockWf[];
extern int _ZTV16daObjFallBlock_c[];
extern int _ZTV10dBgActor_c[];
extern void *data_020a0eac;
int *_ZN11FallBlockWfD0Ev(int *t)
{
    t[0] = (int)_ZTV11FallBlockWf;
    t[0] = (int)_ZTV16daObjFallBlock_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
