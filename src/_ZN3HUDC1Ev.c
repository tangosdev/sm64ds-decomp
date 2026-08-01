// @symbol _ZN3HUDC1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV7dBase_c; VT1 = _ZTV8dMeter_c */
extern void _ZN9ActorBaseC1Ev(void *);
int *_ZN3HUDC1Ev(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(124);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        p[0] = (int)_ZTV7dBase_c;
        p[0] = (int)_ZTV8dMeter_c;
    }
    return p;
}
