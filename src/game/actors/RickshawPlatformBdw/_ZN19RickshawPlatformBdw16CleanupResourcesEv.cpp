//cpp
// @symbol _ZN19RickshawPlatformBdw16CleanupResourcesEv
/* recovered: real C++ method */
/* RickshawPlatformBdw::CleanupResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
#include "RickshawPlatformBdw.h"


extern "C" {
int func_ov002_020b68b0(void *self, void *arg);
extern void *data_ov043_02112418;
}

int RickshawPlatformBdw::CleanupResources()
{
    return func_ov002_020b68b0(this, &data_ov043_02112418);
}
