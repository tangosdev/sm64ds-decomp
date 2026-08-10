//cpp
// @symbol _ZN6Snufit16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the four files InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "Snufit.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov065_0211d610[];
extern int data_ov065_0211d618[];
extern int data_ov065_0211d600[];
extern int data_ov065_0211d608[];
}

int Snufit::CleanupResources()
{
    ((SharedFilePtr *)data_ov065_0211d610)->Release();
    ((SharedFilePtr *)data_ov065_0211d618)->Release();
    ((SharedFilePtr *)data_ov065_0211d600)->Release();
    ((SharedFilePtr *)data_ov065_0211d608)->Release();
    return 1;
}
