//cpp
// @symbol _ZN19daObjKm2_Ukishima_c16CleanupResourcesEv
#include "daObjKm2_Ukishima_c.h"

/* daObjKm2_Ukishima_c::CleanupResources -- vtable slot 3, ov045 0x02111bc8.
 *
 * A tail call into daObjUkiyuka_c's shared cleanup helper
 * func_ov002_020b6424(this, filetable) -- out of this task's scope, kept
 * under its existing name. */
extern "C" {
extern int func_ov002_020b6424(void *self, void *filetable);
extern int data_ov045_02112f08[];
}

int daObjKm2_Ukishima_c::CleanupResources()
{
    return func_ov002_020b6424(this, data_ov045_02112f08);
}
