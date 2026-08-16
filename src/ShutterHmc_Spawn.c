// @symbol ShutterHmc_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV13daObjSwdoor_c[];
extern int _ZTV10ShutterHmc[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjSwdoor_c */
int *ShutterHmc_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(804);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV13daObjSwdoor_c;
        p[0] = (int)_ZTV10ShutterHmc;
    }
    return p;
}
