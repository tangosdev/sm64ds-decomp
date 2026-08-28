//cpp
// @symbol _ZN21ClockPaintingPendulum16CleanupResourcesEv
/* ClockPaintingPendulum::CleanupResources -- vtable slot 3, ov013 0x02111214.
 * Releases the one shared file the painting holds; it never touches `this`. */
#include "ClockPaintingPendulum.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov013_02112280;

s32 ClockPaintingPendulum::CleanupResources()
{
    data_ov013_02112280.Release();
    return 1;
}
