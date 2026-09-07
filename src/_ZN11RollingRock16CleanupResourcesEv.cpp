//cpp
// @symbol _ZN11RollingRock16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 1 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "RollingRock.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov021_02114a50[];
}

int RollingRock::CleanupResources()
{
    ((SharedFilePtr *)data_ov021_02114a50)->Release();
    return 1;
}
