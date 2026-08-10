//cpp
// @symbol _ZN10BowserFire16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1` with no releases, which is the finding rather than a stub:
 * BowserFire holds no SharedFilePtr of its own. Bowser loads and frees the
 * whole fight's files -- 0x1c models, six more, and three singles -- and the
 * fire it breathes borrows from that set without taking a reference.
 */
#include "BowserFire.h"

int BowserFire::CleanupResources()
{
    return 1;
}
