//cpp
// @symbol _ZN7fBase_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "fBase_c.h"
/* fBase_c::Behavior() at 0x02043b24 -- vtable slot 6, per-frame update tick.
 * Base fBase_c does nothing and returns VS_FAIL (1); leaf classes override
 * slot 6 to supply their per-frame behavior.
 */

typedef int s32;

struct fBase_c;

s32 fBase_c::Behavior()
{
    return 1; /* VS_FAIL */
}
