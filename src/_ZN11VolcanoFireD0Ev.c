// @symbol _ZN11VolcanoFireD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV21daObj_volcanoCannon_c */
extern void *data_020a0eac;
int *_ZN11VolcanoFireD0Ev(int *t)
{
    t[0] = (int)_ZTV21daObj_volcanoCannon_c;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
