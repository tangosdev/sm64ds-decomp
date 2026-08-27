//cpp
// @symbol _ZN19RickshawPlatformBdw13InitResourcesEv
/* recovered: real C++ method */
/* RickshawPlatformBdw::InitResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
#include "RickshawPlatformBdw.h"


extern "C" {
int func_ov002_020b6958(void *self, void *arg);
extern void *data_ov043_02112418;
}

int RickshawPlatformBdw::InitResources()
{
    return func_ov002_020b6958(this, &data_ov043_02112418);
}
