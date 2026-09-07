//cpp
// @symbol _ZN9KoopaFlag16CleanupResourcesEv
/* KoopaFlag::CleanupResources -- vtable slot 3. Releases the two shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "KoopaFlag.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov062_0211e0d4;
extern SharedFilePtr data_ov062_0211e0dc;

int KoopaFlag::CleanupResources()
{
    data_ov062_0211e0d4.Release();
    data_ov062_0211e0dc.Release();
    return 1;
}
