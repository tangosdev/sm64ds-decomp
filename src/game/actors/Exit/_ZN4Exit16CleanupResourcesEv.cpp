//cpp
// @symbol _ZN4Exit16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. Exit claims no files -- it is a trigger volume with no
 * model -- so there is nothing to release and the override just reports
 * success.
 */
#include "Exit.h"

int Exit::CleanupResources()
{
    return 1;
}
