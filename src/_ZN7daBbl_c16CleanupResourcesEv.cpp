//cpp
// @symbol _ZN7daBbl_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1` with no release calls, which is the finding rather than a stub:
 * every other class in this overlay releases at least one SharedFilePtr here
 * (daObjShell_c 3, daObjTbox_c 4, JetStream 2, daWater_Ring_c 1). daBbl_c
 * holds none, so it has nothing to give back.
 */
#include "daBbl_c.h"

int daBbl_c::CleanupResources()
{
    return 1;
}
