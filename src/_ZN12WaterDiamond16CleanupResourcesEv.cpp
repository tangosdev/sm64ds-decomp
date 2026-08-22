//cpp
// @symbol _ZN12WaterDiamond16CleanupResourcesEv
/* WaterDiamond::CleanupResources -- vtable slot 3. Releases the one shared file the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "WaterDiamond.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov029_02114270[];
}

int WaterDiamond::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov029_02114270);
    return 1;
}
