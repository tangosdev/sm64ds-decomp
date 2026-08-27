//cpp
// @symbol _ZN20daObjFl_Fall_Block_c13InitResourcesEv
/* daObjFl_Fall_Block_c::InitResources -- vtable slot 0, one of the two slots
 * this class overrides that daObjFallBlock_c leaves null. The body is a
 * delegation to the shared ov098 falling-block setup helper, handing it the
 * Lethal Lava Land descriptor in ov022. */
#include "daObjFl_Fall_Block_c.h"

extern "C" {
int func_ov098_0213a794(void *self, void *desc);
extern int data_ov022_0211427c[];
}

int daObjFl_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(this, data_ov022_0211427c);
}
