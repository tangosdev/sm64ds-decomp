// @symbol RollingLogLll_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV15daObjFlMaruta_c */
int *RollingLogLll_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(836);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV15daObjFlMaruta_c;
        p[0] = (int)VT1;
    }
    return p;
}
