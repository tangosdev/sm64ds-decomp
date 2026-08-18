//cpp
// @symbol _ZN9ActorBase16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"
/* ActorBase::CleanupResources() at 0x02043bf0 -- vtable slot 3.
 * Release files/heap on death. Base returns VS_FAIL (1); leaf classes override.
 */

typedef int s32;

struct ActorBase;

s32 ActorBase::CleanupResources()
{
    return 1; /* VS_FAIL */
}
