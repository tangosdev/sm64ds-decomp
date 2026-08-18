// @symbol _ZN10dBgActor_cC2Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10dBgActor_c */
extern int _ZTV10dBgActor_c[];
int *_ZN10dBgActor_cC2Ev(int *t)
{
    _ZN8dActor_cC2Ev(t);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN5ModelC1Ev((char *)t + 0xd4);
    _ZN18MovingMeshColliderC1Ev((char *)t + 0x124);
    return t;
}
