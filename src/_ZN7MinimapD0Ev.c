// @symbol _ZN7MinimapD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV6dMap_c; VT1 = _ZTV7dBase_c */
extern void _ZN9ActorBaseD2Ev(void *);
extern void *data_020a0eac;
int *_ZN7MinimapD0Ev(int *t)
{
    t[0] = (int)_ZTV6dMap_c;
    t[0] = (int)_ZTV7dBase_c;
    _ZN9ActorBaseD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
