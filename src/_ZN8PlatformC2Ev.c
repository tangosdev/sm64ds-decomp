// @symbol _ZN8PlatformC2Ev
/* recovered: named members + shared header, vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
/* recovered: named members + shared header, vtable identified, globals resolved */
/* resolved: VT0 = _ZTV17ExclamationSwitch */
extern int _ZTV17ExclamationSwitch[];
int *_ZN8PlatformC2Ev(int *t)
{
    _ZN5ActorC2Ev(t);
    t[0] = (int)_ZTV17ExclamationSwitch;
    _ZN5ModelC1Ev((char *)t + 0xd4);
    _ZN18MovingMeshColliderC1Ev((char *)t + 0x124);
    return t;
}
