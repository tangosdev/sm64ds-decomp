//cpp
// @symbol _ZN8MantaRay16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the four shared files InitResources claimed. Two of them live in
 * ov002 rather than this overlay -- the ray borrows assets from the shared
 * pool and still has to release them itself.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "MantaRay.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov090_02134524;
extern SharedFilePtr data_ov002_0210da10;
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov090_0213452c;
}

int MantaRay::CleanupResources()
{
    data_ov090_02134524.Release();
    data_ov002_0210da10.Release();
    data_ov002_0210d9a8.Release();
    data_ov090_0213452c.Release();
    return 1;
}
