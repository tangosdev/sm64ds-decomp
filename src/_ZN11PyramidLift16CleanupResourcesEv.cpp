//cpp
// @symbol _ZN11PyramidLift16CleanupResourcesEv
/* PyramidLift::CleanupResources -- vtable slot 3. Releases the three shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "PyramidLift.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210d9f0[];
extern int data_ov025_02113ae0[];
extern int data_ov025_02113ad8[];
}

int PyramidLift::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9f0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov025_02113ae0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov025_02113ad8);
    return 1;
}
