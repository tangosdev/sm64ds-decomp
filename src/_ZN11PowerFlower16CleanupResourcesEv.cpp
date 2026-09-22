//cpp
// @symbol _ZN11PowerFlower16CleanupResourcesEv
/* PowerFlower::CleanupResources -- vtable slot 3. Releases the two shared files
 * the flower holds; it never touches `this`. */
#include "PowerFlower.h"

extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int gPFlowerCloseModelFile[];
extern int gPFlowerOpenModelFile[];
}

s32 PowerFlower::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(gPFlowerCloseModelFile);
    _ZN13SharedFilePtr7ReleaseEv(gPFlowerOpenModelFile);
    return 1;
}
