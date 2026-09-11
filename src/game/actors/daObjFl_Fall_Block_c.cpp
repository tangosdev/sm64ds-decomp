//cpp
/* Lethal Lava Land's falling block. */

#include "daObjFl_Fall_Block_c.h"

extern "C" {
int func_ov098_0213a2cc(char *self, void **files);
int func_ov098_0213a794(char *self, char **files);
extern int data_ov022_0211427c[];
}

// @symbol _ZN20daObjFl_Fall_Block_c13InitResourcesEv
int daObjFl_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(reinterpret_cast<char *>(this),
                             reinterpret_cast<char **>(data_ov022_0211427c));
}

// @symbol _ZN20daObjFl_Fall_Block_c16CleanupResourcesEv
int daObjFl_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(reinterpret_cast<char *>(this),
                             reinterpret_cast<void **>(data_ov022_0211427c));
}
