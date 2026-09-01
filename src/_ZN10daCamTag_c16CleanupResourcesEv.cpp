//cpp
// @symbol _ZN10daCamTag_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. InitResources claimed nothing, so there is nothing to release --
 * the pair stays balanced by both doing nothing.
 */
#include "daCamTag_c.h"

int daCamTag_c::CleanupResources()
{
    return 1;
}
