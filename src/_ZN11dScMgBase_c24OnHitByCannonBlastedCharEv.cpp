//cpp
#include "dScMgBase_c.h"
// @symbol _ZN11dScMgBase_c24OnHitByCannonBlastedCharEv
// recovered name: dScMgBase_c_OnHitByCannonBlastedChar
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnHitByCannonBlastedChar - slot 26.  The recovered name above is
 * this one's own and is correct; the two overrides' were not, and both are
 * corrected where they live.
 *
 * `return 0;` here, `return 1;` in dScMgSingle3DBase_c's override and
 * `return 2;` in dScMgD3DBase_c's.  Three distinct constants out of three
 * bodies is what makes the return type int rather than void, and it is why the
 * caller has to be able to tell the three apart. */
int dScMgBase_c::OnHitByCannonBlastedChar()
{
    return 0;
}
