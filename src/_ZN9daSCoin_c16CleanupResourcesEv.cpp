//cpp
// @symbol _ZN9daSCoin_c16CleanupResourcesEv
/* daSCoin_c::CleanupResources -- vtable slot 3. Releases the one shared file the
 * class holds and reports success; it never touches `this`, which is why the
 * legacy C form could declare itself nullary and still reproduce. */
#include "daSCoin_c.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210d9a8[];
}

s32 daSCoin_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a8);
    return 1;
}
