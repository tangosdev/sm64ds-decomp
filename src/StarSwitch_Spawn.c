// @symbol StarSwitch_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV10StarSwitch */
int *StarSwitch_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(852);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)_ZTV10StarSwitch; }
    return p;
}
