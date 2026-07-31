// @symbol FloatOnWaterPlatformJrb_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjKi_Ita_c */
int *FloatOnWaterPlatformJrb_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(840);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV13daObjKi_Ita_c;
        p[0] = (int)VT1;
    }
    return p;
}
