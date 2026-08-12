// @symbol MovingBarSmall_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV9MovingBar */
int *MovingBarSmall_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(824);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)_ZTV9MovingBar; }
    return p;
}
