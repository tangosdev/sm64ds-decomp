// @symbol TiltingPlatformLll_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV15daObjGuragura_c[];
extern int _ZTV18TiltingPlatformLll[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV15daObjGuragura_c */
int *TiltingPlatformLll_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(848);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV15daObjGuragura_c;
        p[0] = (int)_ZTV18TiltingPlatformLll;
    }
    return p;
}
