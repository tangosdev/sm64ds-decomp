// @symbol RotatingPlatformRr_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov036_02113b74[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV19daObjRc_Kaitendai_c */
int *RotatingPlatformRr_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV19daObjRc_Kaitendai_c;
        p[0] = (int)data_ov036_02113b74;
    }
    return p;
}
