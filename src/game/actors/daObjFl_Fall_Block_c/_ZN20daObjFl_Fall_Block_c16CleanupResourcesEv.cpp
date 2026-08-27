//cpp
// @symbol _ZN20daObjFl_Fall_Block_c16CleanupResourcesEv
/* daObjFl_Fall_Block_c::CleanupResources -- vtable slot 3, the teardown half of
 * the InitResources delegation, over the same ov022 descriptor. */
#include "daObjFl_Fall_Block_c.h"

extern "C" {
int func_ov098_0213a2cc(void *self, void *desc);
extern int data_ov022_0211427c[];
}

int daObjFl_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, data_ov022_0211427c);
}
