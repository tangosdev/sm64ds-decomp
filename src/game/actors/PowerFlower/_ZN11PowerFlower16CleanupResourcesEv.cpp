//cpp
// @symbol _ZN11PowerFlower16CleanupResourcesEv
/* PowerFlower::CleanupResources -- vtable slot 3. Releases the two shared files
 * the flower holds; it never touches `this`. */
#include "PowerFlower.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov002_0210d9d0[];
extern int data_ov002_0210d9b0[];
}

s32 PowerFlower::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d0);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9b0);
    return 1;
}
