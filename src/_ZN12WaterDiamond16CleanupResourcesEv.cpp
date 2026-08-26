//cpp
// @symbol _ZN12WaterDiamond16CleanupResourcesEv
/* WaterDiamond::CleanupResources -- vtable slot 3. Releases the one shared file the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "WaterDiamond.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov029_02114270;

int WaterDiamond::CleanupResources()
{
    data_ov029_02114270.Release();
    return 1;
}
