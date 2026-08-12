//cpp
// @symbol _ZN9CameraTag16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. InitResources claimed nothing, so there is nothing to release --
 * the pair stays balanced by both doing nothing.
 */
#include "CameraTag.h"

int CameraTag::CleanupResources()
{
    return 1;
}
