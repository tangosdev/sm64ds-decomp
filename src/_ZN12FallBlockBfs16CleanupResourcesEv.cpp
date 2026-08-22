//cpp
// @symbol _ZN12FallBlockBfs16CleanupResourcesEv
/* recovered: real C++ method */
/* FallBlockBfs::CleanupResources() -- forwards `this` and the class's shared parameter block to
 * the generic implementation the family shares. */
#include "FallBlockBfs.h"


extern "C" {
int func_ov098_0213a2cc(void *self, void *data);
extern int data_ov045_021130ac[];
}

int FallBlockBfs::CleanupResources()
{
    return func_ov098_0213a2cc(this, data_ov045_021130ac);
}
