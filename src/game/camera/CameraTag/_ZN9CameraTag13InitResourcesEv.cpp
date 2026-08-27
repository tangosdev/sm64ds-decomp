//cpp
// @symbol _ZN9CameraTag13InitResourcesEv
/* recovered: shared header, real C++ method
 *
 * `return 1`. Nothing to load: the pole has no model and no animation, only
 * the dCcAc_c its spawner already constructed. Reporting success
 * without doing anything is the whole point -- see include/CameraTag.h.
 */
#include "CameraTag.h"

int CameraTag::InitResources()
{
    return 1;
}
