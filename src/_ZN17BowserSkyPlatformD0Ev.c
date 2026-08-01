// @symbol _ZN17BowserSkyPlatformD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV9daKirai_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void *data_020a0eac;
int *_ZN17BowserSkyPlatformD0Ev(int *t)
{
    t[0] = (int)_ZTV9daKirai_c;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
