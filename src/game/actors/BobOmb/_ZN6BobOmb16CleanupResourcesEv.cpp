//cpp
// @symbol _ZN6BobOmb16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the 3 shared file(s) InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "BobOmb.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov002_0210d9e0[];
extern int data_ov102_0214e9c0[];
extern int data_ov102_0214e9c8[];
}

int BobOmb::CleanupResources()
{
    ((SharedFilePtr *)data_ov002_0210d9e0)->Release();
    ((SharedFilePtr *)data_ov102_0214e9c0)->Release();
    ((SharedFilePtr *)data_ov102_0214e9c8)->Release();
    return 1;
}
