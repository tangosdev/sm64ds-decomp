//cpp
// @symbol _ZN11VirtualDoor16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. VirtualDoor claims no files -- it is a trigger volume with no
 * model -- so there is nothing to release and the override just reports
 * success.
 */
#include "VirtualDoor.h"

int VirtualDoor::CleanupResources()
{
    return 1;
}
