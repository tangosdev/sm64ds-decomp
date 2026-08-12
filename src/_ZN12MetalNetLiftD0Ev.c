// @symbol _ZN12MetalNetLiftD0Ev
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV12MetalNetLift[];
extern int _ZTV8Platform[];
/* recovered: vtable identified, renamed to Class_Method */
extern void *data_020a0eac;
int *_ZN12MetalNetLiftD0Ev(int *t)
{
    t[0] = (int)_ZTV12MetalNetLift;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
