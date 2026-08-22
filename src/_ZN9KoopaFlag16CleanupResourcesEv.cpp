//cpp
// @symbol _ZN9KoopaFlag16CleanupResourcesEv
/* KoopaFlag::CleanupResources -- vtable slot 3. Releases the two shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "KoopaFlag.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov062_0211e0d4[];
extern int data_ov062_0211e0dc[];
}

int KoopaFlag::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211e0d4);
    _ZN13SharedFilePtr7ReleaseEv(data_ov062_0211e0dc);
    return 1;
}
