//cpp
// @symbol _ZN12HauntedChair16CleanupResourcesEv
/* HauntedChair::CleanupResources -- vtable slot 3. Releases the one shared file the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "HauntedChair.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov020_02114af0;
}

int HauntedChair::CleanupResources()
{
    data_ov020_02114af0.Release();
    return 1;
}
