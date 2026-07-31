//cpp
// @symbol _ZN9ActorBase13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"
/* ActorBase::InitResources() at 0x02043c80 -- vtable slot 0, one-time setup.
 * Base ActorBase does nothing and returns VS_FAIL (1); leaf classes override
 * slot 0 to load models/anims/collision.
 */

typedef int s32;

struct ActorBase;

s32 ActorBase::InitResources()
{
    return 1; /* VS_FAIL */
}
