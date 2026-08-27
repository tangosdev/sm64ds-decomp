//cpp
// @symbol _ZN5Swoop16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the four files InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "Swoop.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov065_0211d698[];
extern int data_ov065_0211d6a8[];
extern int data_ov065_0211d690[];
extern int data_ov065_0211d6a0[];
}

int Swoop::CleanupResources()
{
    ((SharedFilePtr *)data_ov065_0211d698)->Release();
    ((SharedFilePtr *)data_ov065_0211d6a8)->Release();
    ((SharedFilePtr *)data_ov065_0211d690)->Release();
    ((SharedFilePtr *)data_ov065_0211d6a0)->Release();
    return 1;
}
