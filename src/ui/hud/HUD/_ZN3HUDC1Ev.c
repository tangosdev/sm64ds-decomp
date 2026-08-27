// @symbol _ZN3HUDC1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int _ZTV3HUD[];
extern int _ZTV7dBase_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV7dBase_c; VT1 = _ZTV3HUD */
extern void _ZN7fBase_cC2Ev(void *);
int *_ZN3HUDC1Ev(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(124);
    if (p) {
        _ZN7fBase_cC2Ev(p);
        p[0] = (int)_ZTV7dBase_c;
        p[0] = (int)_ZTV3HUD;
    }
    return p;
}
