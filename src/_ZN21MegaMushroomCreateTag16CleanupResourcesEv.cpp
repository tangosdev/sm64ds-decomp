//cpp
// @symbol _ZN21MegaMushroomCreateTag16CleanupResourcesEv
/* MegaMushroomCreateTag::CleanupResources -- vtable slot 3. Releases the one shared file the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "MegaMushroomCreateTag.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210da30[];
}

int MegaMushroomCreateTag::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da30);
    return 1;
}
