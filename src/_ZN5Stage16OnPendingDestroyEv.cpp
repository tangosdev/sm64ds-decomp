//cpp
// @symbol _ZN5Stage16OnPendingDestroyEv
/* recovered: named members + shared header, real C++ method */
#include "Stage.h"
/* Stage::OnPendingDestroy() at 0x0202b8a0
 * fBase_c vtable slot 12 (0x30), called when the Stage scene is marked for
 * destruction, before CleanupResources(). Stage overrides it with an empty body
 * -- no teardown work needed at this point.
 * `this` is Stage* (Stage derives from dScene_c <- dBase_c <- fBase_c).
 */

struct Stage;

void Stage::OnPendingDestroy()
{
    (void)this;
}
