//cpp
// @symbol _ZN11PyramidLift16CleanupResourcesEv
/* PyramidLift::CleanupResources -- vtable slot 3. Releases the three shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "PyramidLift.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov002_0210d9f0;
extern SharedFilePtr data_ov025_02113ae0;
extern SharedFilePtr data_ov025_02113ad8;
}

int PyramidLift::CleanupResources()
{
    data_ov002_0210d9f0.Release();
    data_ov025_02113ae0.Release();
    data_ov025_02113ad8.Release();
    return 1;
}
