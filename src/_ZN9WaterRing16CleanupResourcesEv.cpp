//cpp
// @symbol _ZN9WaterRing16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * One release, of a handle that lives in ov002 rather than in this overlay --
 * the ring borrows a model the always-resident module owns.
 *
 * `SharedFilePtr::Release` is now spelt as the member call it is, so the
 * compiler emits _ZN13SharedFilePtr7ReleaseEv itself.
 */
#include "WaterRing.h"
#include "SharedFilePtr.h"

extern char data_ov002_0210da10;

int WaterRing::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov002_0210da10))->Release();
    return 1;
}
