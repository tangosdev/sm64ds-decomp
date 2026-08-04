// @symbol RotatingPlatformWf_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov015_021147e8[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjBk_Ukisima_c */
int *RotatingPlatformWf_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV17daObjBk_Ukisima_c;
        p[0] = (int)data_ov015_021147e8;
    }
    return p;
}
