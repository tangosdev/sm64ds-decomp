// @symbol _ZN7MinimapC1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int _ZTV12ActorDerived[];
extern int _ZTV7Minimap[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV12ActorDerived; VT1 = _ZTV7Minimap */
extern void _ZN9ActorBaseC1Ev(void *);
int *_ZN7MinimapC1Ev(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(600);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        p[0] = (int)_ZTV12ActorDerived;
        p[0] = (int)_ZTV7Minimap;
    }
    return p;
}
