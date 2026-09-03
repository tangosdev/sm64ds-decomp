//cpp
// @symbol _ZN9daRFlag_c16CleanupResourcesEv
/* daRFlag_c::CleanupResources -- vtable slot 3. Releases the two shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "daRFlag_c.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov062_0211e0d4;
extern SharedFilePtr data_ov062_0211e0dc;

int daRFlag_c::CleanupResources()
{
    data_ov062_0211e0d4.Release();
    data_ov062_0211e0dc.Release();
    return 1;
}
