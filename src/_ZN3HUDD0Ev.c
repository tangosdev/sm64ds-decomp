// @symbol _ZN3HUDD0Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_common.h"
extern int _ZTV3HUD[];
extern int _ZTV7dBase_c[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV3HUD; VT1 = _ZTV7dBase_c */
extern void _ZN7fBase_cD2Ev(void *);
extern void *data_020a0eac;
int *_ZN3HUDD0Ev(int *t)
{
    t[0] = (int)_ZTV3HUD;
    t[0] = (int)_ZTV7dBase_c;
    _ZN7fBase_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
