// @symbol RickshawPlatformBs_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV13daObjKuruma_c[];
extern int data_ov047_0211244c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjKuruma_c */
int *RickshawPlatformBs_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV13daObjKuruma_c;
        p[0] = (int)data_ov047_0211244c;
    }
    return p;
}
