//cpp
// @symbol _ZN13dScGameOver_c16CleanupResourcesEv
/* recovered: real C++ method */
/* dScGameOver_c::CleanupResources() -- vtable slot 3. The game-over scene owns
 * no allocation of its own: everything it draws is OAM data belonging to the
 * overlay, so there is nothing to release. */
#include "dScGameOver_c.h"

s32 dScGameOver_c::CleanupResources()
{
    return 1;
}
