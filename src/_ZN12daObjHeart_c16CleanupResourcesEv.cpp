//cpp
// @symbol _ZN12daObjHeart_c16CleanupResourcesEv
/* daObjHeart_c::CleanupResources -- vtable slot 3. Releases the two shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "daObjHeart_c.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210e104[];
extern int data_ov002_0210e0fc[];
}

int daObjHeart_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210e104);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210e0fc);
    return 1;
}
