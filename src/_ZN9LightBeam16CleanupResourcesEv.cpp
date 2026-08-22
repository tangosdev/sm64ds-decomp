//cpp
// @symbol _ZN9LightBeam16CleanupResourcesEv
/* LightBeam::CleanupResources -- vtable slot 3. Releases the one shared file the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "LightBeam.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov010_02112d50[];
}

int LightBeam::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov010_02112d50);
    return 1;
}
