// @symbol RollingLogTtm_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV13RollingLogTtm[];
extern int _ZTV13daObjMaruta_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjMaruta_c */
int *RollingLogTtm_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(836);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV13daObjMaruta_c;
        p[0] = (int)_ZTV13RollingLogTtm;
    }
    return p;
}
