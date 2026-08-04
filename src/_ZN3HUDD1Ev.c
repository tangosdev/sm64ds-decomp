// @symbol _ZN3HUDD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_common.h"
extern int _ZTV3HUD[];
extern int _ZTV12ActorDerived[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV3HUD; VT1 = _ZTV12ActorDerived */
extern void _ZN9ActorBaseD2Ev(void *);
int *_ZN3HUDD1Ev(int *t)
{
    t[0] = (int)_ZTV3HUD;
    t[0] = (int)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(t);
    return t;
}
