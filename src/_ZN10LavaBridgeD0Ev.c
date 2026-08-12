// @symbol _ZN10LavaBridgeD0Ev
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV10LavaBridge[];
extern int _ZTV8Platform[];
/* recovered: vtable identified, renamed to Class_Method */
/* The DELETING DESTRUCTOR, vtable slot 17. The old comment here called it
   OnYoshiTryEat, which is slot 18 -- off by one. This body stores the class
   vtable, destroys the members and calls Memory::Deallocate; no eat handler
   does any of that. */
extern void *data_020a0eac;
int *_ZN10LavaBridgeD0Ev(int *t)
{
    t[0] = (int)_ZTV10LavaBridge;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
