//cpp
// @symbol _ZN7Minimap16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Minimap.h"
/* Minimap::CleanupResources() at 0x020f9e8c (ov002) -- vtable slot 3.
 * Returns VS_FAIL (1); the minimap holds no SharedFilePtr/heap resources
 * to release on death. Minimap : ActorDerived : ActorBase.
 */

typedef int s32;

struct Minimap;

s32 Minimap::CleanupResources()
{
    (void)this;
    return 1;
}
