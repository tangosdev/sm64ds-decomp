// @symbol RollingLogTtm_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV9UkikiCage[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV15daObjHmMaruta_c */
int *RollingLogTtm_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(836);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV15daObjHmMaruta_c;
        p[0] = (int)_ZTV9UkikiCage;
    }
    return p;
}
