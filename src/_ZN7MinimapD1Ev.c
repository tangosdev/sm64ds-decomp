// @symbol _ZN7MinimapD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_common.h"
extern int _ZTV12ActorDerived[];
extern int _ZTV7Minimap[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV7Minimap; VT1 = _ZTV12ActorDerived */
extern void _ZN9ActorBaseD2Ev(void *);
int *_ZN7MinimapD1Ev(int *t)
{
    t[0] = (int)_ZTV7Minimap;
    t[0] = (int)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(t);
    return t;
}
