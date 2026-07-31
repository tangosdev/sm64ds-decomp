// @symbol _ZN3HUDD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV8dMeter_c; VT1 = _ZTV7dBase_c */
extern void _ZN9ActorBaseD2Ev(void *);
int *_ZN3HUDD1Ev(int *t)
{
    t[0] = (int)_ZTV8dMeter_c;
    t[0] = (int)_ZTV7dBase_c;
    _ZN9ActorBaseD2Ev(t);
    return t;
}
