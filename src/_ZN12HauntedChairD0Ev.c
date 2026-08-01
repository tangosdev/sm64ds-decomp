// @symbol _ZN12HauntedChairD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV9daChair_c */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void *data_020a0eac;
int *_ZN12HauntedChairD0Ev(int *t)
{
    t[0] = (int)_ZTV9daChair_c;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1bc);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x17c);
    _ZN11ShadowModelD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
