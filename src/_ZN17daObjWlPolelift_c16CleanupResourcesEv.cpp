//cpp
// @symbol _ZN17daObjWlPolelift_c16CleanupResourcesEv
/* daObjWlPolelift_c::CleanupResources -- vtable slot 3. Releases the one shared
 * file the class holds and reports success; it never touches `this`. */
#include "daObjWlPolelift_c.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov026_02113ea0;

s32 daObjWlPolelift_c::CleanupResources()
{
    data_ov026_02113ea0.Release();
    return 1;
}
