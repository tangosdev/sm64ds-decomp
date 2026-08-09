//cpp
// @symbol _ZN13InvisiblePole16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. InitResources claimed nothing, so there is nothing to release --
 * the pair stays balanced by both doing nothing.
 */
#include "InvisiblePole.h"

int InvisiblePole::CleanupResources()
{
    return 1;
}
