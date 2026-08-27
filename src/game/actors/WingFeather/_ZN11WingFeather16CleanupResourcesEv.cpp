//cpp
// @symbol _ZN11WingFeather16CleanupResourcesEv
/* WingFeather::CleanupResources -- vtable slot 3. Releases the one shared file the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "WingFeather.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210da58[];
}

int WingFeather::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da58);
    return 1;
}
