//cpp
// @symbol _ZN10daCamTag_c13InitResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. Nothing to load: the pole has no model and no animation, only
 * the dCcAc_c its spawner already constructed. Reporting success
 * without doing anything is the whole point -- see include/daCamTag_c.h.
 */
#include "daCamTag_c.h"

int daCamTag_c::InitResources()
{
    return 1;
}
