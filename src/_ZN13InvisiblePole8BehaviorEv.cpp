//cpp
// @symbol _ZN13InvisiblePole8BehaviorEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. The pole never moves and never thinks; it is queried, not
 * ticked.
 */
#include "InvisiblePole.h"

int InvisiblePole::Behavior()
{
    return 1;
}
