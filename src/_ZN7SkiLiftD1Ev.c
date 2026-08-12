// @symbol _ZN7SkiLiftD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV7SkiLift[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV7SkiLift; VT1 = _ZTV8Platform */
int *_ZN7SkiLiftD1Ev(int *t)
{
    t[0] = (int)_ZTV7SkiLift;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
