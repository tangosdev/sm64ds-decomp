//cpp
// @symbol _ZN21ClockPaintingPendulum16CleanupResourcesEv
/* ClockPaintingPendulum::CleanupResources -- vtable slot 3, ov013 0x02111214.
 * Releases the one shared file the painting holds; it never touches `this`. */
#include "ClockPaintingPendulum.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov013_02112280[];
}

s32 ClockPaintingPendulum::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov013_02112280);
    return 1;
}
