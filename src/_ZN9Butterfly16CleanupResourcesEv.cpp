//cpp
// @symbol _ZN9Butterfly16CleanupResourcesEv
/* Butterfly::CleanupResources -- vtable slot 3. Releases the four shared files
 * the class holds -- three of its own in ov100 and one shared with the other
 * small fauna in ov002; it never touches `this`. */
#include "Butterfly.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov100_02148608;
extern SharedFilePtr data_ov100_02148600;
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov100_02148668;
}

int Butterfly::CleanupResources()
{
    data_ov100_02148608.Release();
    data_ov100_02148600.Release();
    data_ov002_0210d9d8.Release();
    data_ov100_02148668.Release();
    return 1;
}
