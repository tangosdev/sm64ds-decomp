//cpp
// @symbol _ZN12FallBlockBfs13InitResourcesEv
/* recovered: real C++ method */
/* FallBlockBfs::InitResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
#include "FallBlockBfs.h"


extern "C" {
int func_ov098_0213a794(void *self, void *data);
extern int data_ov045_021130ac[];
}

int FallBlockBfs::InitResources()
{
    return func_ov098_0213a794(this, data_ov045_021130ac);
}
