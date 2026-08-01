//cpp
// @symbol _ZN9ActorBase8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"
/* ActorBase::Behavior() at 0x02043b24 -- vtable slot 6, per-frame update tick.
 * Base ActorBase does nothing and returns VS_FAIL (1); leaf classes override
 * slot 6 to supply their per-frame behavior.
 */

typedef int s32;

struct ActorBase;

s32 ActorBase::Behavior()
{
    return 1; /* VS_FAIL */
}
