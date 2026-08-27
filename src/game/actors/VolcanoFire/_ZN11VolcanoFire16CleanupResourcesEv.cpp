//cpp
// @symbol _ZN11VolcanoFire16CleanupResourcesEv
/* VolcanoFire::CleanupResources -- vtable slot 3. The only one of these leaves
 * that owns no files: it hands its slot back to the spawner that made it, by
 * decrementing the live-flame count that spawner keeps at +0x324. mSpawner is
 * null for a flame that was never registered, hence the guard. The spawner's
 * own class is not described by any header yet, so that offset stays raw. */
#include "VolcanoFire.h"

int VolcanoFire::CleanupResources()
{
    if (mSpawner)
        *(u16 *)((char *)mSpawner + 0x324) -= 1;
    return 1;
}
