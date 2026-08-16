// @symbol RotatingPlatformWdw_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV16daObjKaitendai_c[];
extern int _ZTV19RotatingPlatformWdw[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16daObjKaitendai_c */
int *RotatingPlatformWdw_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV16daObjKaitendai_c;
        p[0] = (int)_ZTV19RotatingPlatformWdw;
    }
    return p;
}
