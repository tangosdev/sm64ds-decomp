// @symbol _ZN12MansionStepsD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV11daTrsTrap_c */
extern void *G0;
int *_ZN12MansionStepsD0Ev(int *t)
{
    t[0] = (int)_ZTV11daTrsTrap_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x15c);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
