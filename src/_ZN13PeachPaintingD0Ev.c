// @symbol _ZN13PeachPaintingD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV14daObjC1Peach_c */
extern void *G0;
int *_ZN13PeachPaintingD0Ev(int *t)
{
    t[0] = (int)_ZTV14daObjC1Peach_c;
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
