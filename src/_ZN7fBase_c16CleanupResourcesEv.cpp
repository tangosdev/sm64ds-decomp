//cpp
// @symbol _ZN7fBase_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "fBase_c.h"
/* fBase_c::CleanupResources() at 0x02043bf0 -- vtable slot 3.
 * Release files/heap on death. Base returns VS_FAIL (1); leaf classes override.
 */

typedef int s32;

struct fBase_c;

s32 fBase_c::CleanupResources()
{
    return 1; /* VS_FAIL */
}
