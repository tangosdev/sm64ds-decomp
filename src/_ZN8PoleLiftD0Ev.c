// @symbol _ZN8PoleLiftD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ExtendingMeshCollider.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV17daObjKm2_Nobiru_c */
extern void *G0;
int *_ZN8PoleLiftD0Ev(int *t)
{
    t[0] = (int)_ZTV17daObjKm2_Nobiru_c;
    _ZN21ExtendingMeshColliderD1Ev((char *)t + 0x158);
    _ZN5ModelD1Ev((char *)t + 0xd8);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
