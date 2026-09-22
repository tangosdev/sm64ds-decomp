//cpp
// @symbol _ZN9WaterRing16OnPendingDestroyEv
/* recovered: shared header, real C++ method
 *
 * Empty -- the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy, not to do anything itself.
 */
#include "WaterRing.h"

void WaterRing::OnPendingDestroy()
{
}
