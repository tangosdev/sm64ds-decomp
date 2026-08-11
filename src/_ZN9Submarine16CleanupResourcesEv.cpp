//cpp
// @symbol _ZN9Submarine16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 2 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "Submarine.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov026_02113f0c[];
extern int data_ov026_02113f04[];
}

int Submarine::CleanupResources()
{
    ((SharedFilePtr *)data_ov026_02113f0c)->Release();
    ((SharedFilePtr *)data_ov026_02113f04)->Release();
    return 1;
}
