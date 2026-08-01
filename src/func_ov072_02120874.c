// @symbol func_ov072_02120874
// recovered name: daBgSnwmn_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daBgSnwmn_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *G0;
int *func_ov072_02120874(int *t)
{
    t[0] = (int)VT0;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0x1b0);
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN15TextureSequenceD1Ev((char *)t + 0x174);
    _ZN5ModelD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
