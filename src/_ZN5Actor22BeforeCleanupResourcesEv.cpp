//cpp
// @symbol _ZN5Actor22BeforeCleanupResourcesEv
/* Actor::BeforeCleanupResources() at 0x02011220 -- vtable slot 4.
 *
 * Normalises the ActorBase result to 0 or 1.
 *
 * Kept as a .c filename with the //cpp marker; see the note in
 * src/_ZN5Actor18AfterInitResourcesEj.c.
 */
#include "Actor.h"

int Actor::BeforeCleanupResources()
{
    if (ActorBase::BeforeCleanupResources() != 0)
        return 1;
    return 0;
}
