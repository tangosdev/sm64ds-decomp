// @symbol _ZN3HUDC1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int _ZTV3HUD[];
extern int _ZTV12ActorDerived[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV12ActorDerived; VT1 = _ZTV3HUD */
extern void _ZN9ActorBaseC1Ev(void *);
int *_ZN3HUDC1Ev(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(124);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        p[0] = (int)_ZTV12ActorDerived;
        p[0] = (int)_ZTV3HUD;
    }
    return p;
}
