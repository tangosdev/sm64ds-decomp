//cpp
// @symbol _ZN12HauntedChair16CleanupResourcesEv
/* HauntedChair::CleanupResources -- vtable slot 3. Releases the one shared file the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "HauntedChair.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov020_02114af0[];
}

int HauntedChair::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov020_02114af0);
    return 1;
}
