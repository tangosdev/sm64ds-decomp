// @symbol _ZN7MinimapC1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int _ZTV7dBase_c[];
extern int _ZTV7Minimap[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV7dBase_c; VT1 = _ZTV7Minimap */
extern void _ZN7fBase_cC2Ev(void *);
int *_ZN7MinimapC1Ev(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(600);
    if (p) {
        _ZN7fBase_cC2Ev(p);
        p[0] = (int)_ZTV7dBase_c;
        p[0] = (int)_ZTV7Minimap;
    }
    return p;
}
