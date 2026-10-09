//cpp
// @symbol _ZN10dScStage_c16OnPendingDestroyEv
/* recovered: named members + shared header, real C++ method */
#include "dScStage_c.h"
/* dScStage_c::OnPendingDestroy() at 0x0202b8a0
 * fBase_c vtable slot 12 (0x30), called when the dScStage_c scene is marked for
 * destruction, before CleanupResources(). dScStage_c overrides it with an empty body
 * -- no teardown work needed at this point.
 * `this` is dScStage_c* (dScStage_c derives from dScene_c <- dBase_c <- fBase_c).
 */

struct dScStage_c;

void dScStage_c::OnPendingDestroy()
{
    (void)this;
}
