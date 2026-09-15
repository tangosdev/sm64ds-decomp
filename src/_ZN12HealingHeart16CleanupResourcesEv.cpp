//cpp
// @symbol _ZN12HealingHeart16CleanupResourcesEv
/* HealingHeart::CleanupResources -- vtable slot 3. Releases the two shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "HealingHeart.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov002_0210e104;
extern SharedFilePtr data_ov002_0210e0fc;
}

int HealingHeart::CleanupResources()
{
    data_ov002_0210e104.Release();
    data_ov002_0210e0fc.Release();
    return 1;
}
