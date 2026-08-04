// @symbol _ZN20SwitchActivatedPlankD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV8Platform[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV15daObjWc_Obj04_c; VT1 = _ZTV8Platform */
extern void *data_020a0eac;
int *_ZN20SwitchActivatedPlankD0Ev(int *t)
{
    t[0] = (int)_ZTV15daObjWc_Obj04_c;
    _ZN5ModelD1Ev((char *)t + 0x320);
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
