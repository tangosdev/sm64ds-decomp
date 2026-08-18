// @symbol _ZN9ArrowLiftD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV9ArrowLift[];
extern int _ZTV10dBgActor_c[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV9ArrowLift; VT1 = _ZTV10dBgActor_c */
extern void *data_020a0eac;
int *_ZN9ArrowLiftD0Ev(int *t)
{
    t[0] = (int)_ZTV9ArrowLift;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
