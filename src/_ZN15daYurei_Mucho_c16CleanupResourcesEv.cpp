//cpp
// @symbol _ZN15daYurei_Mucho_c16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases the four files InitResources claimed.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "daYurei_Mucho_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov065_0211d610[];
extern int data_ov065_0211d618[];
extern int data_ov065_0211d600[];
extern int data_ov065_0211d608[];
}

int daYurei_Mucho_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov065_0211d610)->Release();
    ((SharedFilePtr *)data_ov065_0211d618)->Release();
    ((SharedFilePtr *)data_ov065_0211d600)->Release();
    ((SharedFilePtr *)data_ov065_0211d608)->Release();
    return 1;
}
