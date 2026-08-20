// @symbol _ZN9daSCoin_cD1Ev
/* recovered: vtable identified, declarations from a shared header. Was
 * _ZN15InvisibleSecretD1Ev -- renamed to match the RTTI class name
 * (_ZTS9daSCoin_c), see include/daSCoin_c.h. */
#include "decl_Actor.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
extern int _ZTV9daSCoin_c[];
/* vtable identified: VT0 = _ZTV9daSCoin_c */
int *_ZN9daSCoin_cD1Ev(int *t)
{
    t[0] = (int)_ZTV9daSCoin_c;
    _ZN7dCcAc_cD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
