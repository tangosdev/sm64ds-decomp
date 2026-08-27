//cpp
// @symbol _ZN18TiltingPlatformBfs16CleanupResourcesEv
/* recovered: real C++ method */
/* TiltingPlatformBfs::CleanupResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
#include "TiltingPlatformBfs.h"


extern "C" {
int func_ov002_020b60fc(void *self, void *data);
extern int data_ov045_02112fdc[];
}

int TiltingPlatformBfs::CleanupResources()
{
    return func_ov002_020b60fc(this, data_ov045_02112fdc);
}
