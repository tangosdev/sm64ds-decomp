//cpp
// @symbol _ZN7Tornado16CleanupResourcesEv
/* Tornado::CleanupResources -- vtable slot 3. Releases the two shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "Tornado.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov096_02137ba8;
extern SharedFilePtr data_ov096_02137bb0;
}

int Tornado::CleanupResources()
{
    data_ov096_02137ba8.Release();
    data_ov096_02137bb0.Release();
    return 1;
}
