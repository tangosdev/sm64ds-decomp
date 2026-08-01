// @symbol VirtualDoor_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV9CameraTag */
int *VirtualDoor_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(212);
    if (p) { _ZN5ActorC2Ev(p); p[0] = (int)_ZTV9CameraTag; }
    return p;
}
