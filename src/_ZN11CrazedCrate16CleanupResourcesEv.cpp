//cpp
// @symbol _ZN11CrazedCrate16CleanupResourcesEv
/* recovered: real C++ method */
/* CrazedCrate::CleanupResources() -- vtable slot 3. One shared file handle to
 * give back. */
#include "CrazedCrate.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov080_02128468;
}

s32 CrazedCrate::CleanupResources()
{
    data_ov080_02128468.Release();
    return 1;
}
