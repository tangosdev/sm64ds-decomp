//cpp
// @symbol _ZN10Scuttlebug16CleanupResourcesEv
/* Scuttlebug::CleanupResources -- vtable slot 3. Releases the two shared files
 * the class holds; it never touches `this`, which is why the legacy C form could
 * declare itself nullary and still reproduce. */
#include "Scuttlebug.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov071_02122f80[];
extern int data_ov071_02122f88[];
}

int Scuttlebug::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov071_02122f80);
    _ZN13SharedFilePtr7ReleaseEv(data_ov071_02122f88);
    return 1;
}
