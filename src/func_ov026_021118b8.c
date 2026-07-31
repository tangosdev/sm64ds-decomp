// @symbol func_ov026_021118b8
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV18daObjWlSubmarine_c; VT1 = _ZTV10dBgActor_c */
int *func_ov026_021118b8(int *t)
{
    t[0] = (int)_ZTV18daObjWlSubmarine_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
