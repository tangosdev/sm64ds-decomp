//cpp
// @symbol _ZN13InvisiblePole13InitResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. Nothing to load: the pole has no model and no animation, only
 * the MovingCylinderClsn its spawner already constructed. Reporting success
 * without doing anything is the whole point -- see include/InvisiblePole.h.
 */
#include "InvisiblePole.h"

int InvisiblePole::InitResources()
{
    return 1;
}
