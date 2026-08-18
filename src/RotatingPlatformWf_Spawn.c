// @symbol RotatingPlatformWf_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV16daObjKaitendai_c[];
extern int _ZTV18RotatingPlatformWf[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16daObjKaitendai_c */
int *RotatingPlatformWf_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjKaitendai_c;
        p[0] = (int)_ZTV18RotatingPlatformWf;
    }
    return p;
}
