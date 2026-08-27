// @symbol daObjAbuku_c_Spawn
/* recovered: vtable identified, declarations from a shared header. Was
 * Bubble_Spawn -- renamed to match the RTTI class name
 * (_ZTS12daObjAbuku_c), see include/daObjAbuku_c.h. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
extern int _ZTV12daObjAbuku_c[];
/* vtable identified: VT0 = _ZTV12daObjAbuku_c */
int *daObjAbuku_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(276);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV12daObjAbuku_c;
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
