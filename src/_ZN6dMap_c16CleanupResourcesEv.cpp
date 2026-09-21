//cpp
// @symbol _ZN6dMap_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "dMap_c.h"
/* dMap_c::CleanupResources() at 0x020f9e8c (ov002) -- vtable slot 3.
 * Returns VS_FAIL (1); the minimap holds no SharedFilePtr/heap resources
 * to release on death. dMap_c : dBase_c : fBase_c.
 */

typedef int s32;

struct dMap_c;

s32 dMap_c::CleanupResources()
{
    (void)this;
    return 1;
}
