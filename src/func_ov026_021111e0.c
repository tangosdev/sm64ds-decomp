// @symbol func_ov026_021111e0
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov026_02113ae0[];
/* recovered: vtable identified, renamed to Class_Method */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void *data_020a0eac;
int *func_ov026_021111e0(int *t)
{
    t[0] = (int)data_ov026_02113ae0;
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
