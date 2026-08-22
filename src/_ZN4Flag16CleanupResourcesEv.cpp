//cpp
// @symbol _ZN4Flag16CleanupResourcesEv
/* Flag::CleanupResources -- vtable slot 3. Releases the two shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "Flag.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov009_02113eb8[];
extern int data_ov009_02113eb0[];
}

int Flag::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov009_02113eb8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov009_02113eb0);
    return 1;
}
