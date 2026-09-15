//cpp
// @symbol _ZN11PowerFlower16CleanupResourcesEv
/* PowerFlower::CleanupResources -- vtable slot 3. Releases the two shared files
 * the flower holds; it never touches `this`. */
#include "PowerFlower.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr gPFlowerCloseModelFile;
extern SharedFilePtr gPFlowerOpenModelFile;
}

s32 PowerFlower::CleanupResources()
{
    gPFlowerCloseModelFile.Release();
    gPFlowerOpenModelFile.Release();
    return 1;
}
