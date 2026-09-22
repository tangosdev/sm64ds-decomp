//cpp
// @symbol _ZN8Snowball16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 1 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "Snowball.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov081_02128d90[];
}

int Snowball::CleanupResources()
{
    ((SharedFilePtr *)data_ov081_02128d90)->Release();
    return 1;
}
