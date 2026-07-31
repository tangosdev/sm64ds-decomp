// @symbol _ZN14MovingBarSmallD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV14daObjBk_Lift_c; VT1 = _ZTV10dBgActor_c */
int *_ZN14MovingBarSmallD1Ev(int *t)
{
    t[0] = (int)_ZTV14daObjBk_Lift_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x320);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
