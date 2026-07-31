// @symbol func_ov025_02111928
// @emits daDkk_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
/* daDkk_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *G0;
int *daDkk_c_OnYoshiTryEat(int *t)
{
    t[0] = (int)VT0;
    t[0] = (int)VT1;
    _ZN11ShadowModelD1Ev((char *)t + 0x338);
    _ZN15TextureSequenceD1Ev((char *)t + 0x324);
    t[0] = (int)VT2;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, G0);
    return t;
}
