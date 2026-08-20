// @symbol _ZN12daObjAbuku_cD1Ev
/* recovered: vtable identified, declarations from a shared header. Was
 * _ZN6BubbleD1Ev -- renamed to match the RTTI class name
 * (_ZTS12daObjAbuku_c), see include/daObjAbuku_c.h. */
#include "decl_Actor.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
extern int _ZTV12daObjAbuku_c[];
/* vtable identified: VT0 = _ZTV12daObjAbuku_c */
int *_ZN12daObjAbuku_cD1Ev(int *t)
{
    t[0] = (int)_ZTV12daObjAbuku_c;
    _ZN7dCcAc_cD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
