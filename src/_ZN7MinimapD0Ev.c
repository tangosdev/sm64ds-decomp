// @symbol _ZN7MinimapD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_common.h"
extern int _ZTV12ActorDerived[];
extern int _ZTV7Minimap[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV7Minimap; VT1 = _ZTV12ActorDerived */
extern void _ZN9ActorBaseD2Ev(void *);
extern void *data_020a0eac;
int *_ZN7MinimapD0Ev(int *t)
{
    t[0] = (int)_ZTV7Minimap;
    t[0] = (int)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
