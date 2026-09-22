//cpp
// @symbol _ZN7Tornado16CleanupResourcesEv
/* Tornado::CleanupResources -- vtable slot 3. Releases the two shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "Tornado.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov096_02137ba8[];
extern int data_ov096_02137bb0[];
}

int Tornado::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137ba8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137bb0);
    return 1;
}
