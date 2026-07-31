// @symbol _ZN11VirtualDoorD0Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "VirtualDoor.h"
int *_ZN11VirtualDoorD0Ev(int *t)
{
    t[0] = (int)VT;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, HEAP);
    return t;
}
