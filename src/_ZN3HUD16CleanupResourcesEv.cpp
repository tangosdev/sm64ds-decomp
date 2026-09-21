//cpp
// @symbol _ZN8dMeter_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "dMeter_c.h"
/* dMeter_c::CleanupResources() at 0x020fd5d4 (ov002) -- vtable slot 3.
 * Returns VS_FAIL (1); the dMeter_c holds no SharedFilePtr/heap resources to
 * release on death. dMeter_c : dBase_c : fBase_c.
 */

typedef int s32;

struct dMeter_c;

s32 dMeter_c::CleanupResources()
{
    (void)this;
    return 1;
}
