//cpp
// @symbol _ZN3HUD16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "HUD.h"
/* HUD::CleanupResources() at 0x020fd5d4 (ov002) -- vtable slot 3.
 * Returns VS_FAIL (1); the HUD holds no SharedFilePtr/heap resources to
 * release on death. HUD : dBase_c : fBase_c.
 */

typedef int s32;

struct HUD;

s32 HUD::CleanupResources()
{
    (void)this;
    return 1;
}
