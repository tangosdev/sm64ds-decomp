// @symbol _ZN18BowserFireSeaArenaD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV8Platform[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV10daKpa2Bg_c; VT1 = _ZTV8Platform */
int *_ZN18BowserFireSeaArenaD1Ev(int *t)
{
    t[0] = (int)_ZTV10daKpa2Bg_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x374);
    _ZN5ModelD1Ev((char *)t + 0x324);
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
