// @symbol CameraTag_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV13InvisiblePole */
int *CameraTag_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(212);
    if (p) { _ZN5ActorC2Ev(p); p[0] = (int)_ZTV13InvisiblePole; }
    return p;
}
