//cpp
// @symbol _ZN12ActorDerived18AfterInitResourcesEj
/* ActorDerived::AfterInitResources(u32) at 0x02013ef4 -- vtable slot 2, the only
 * functional override this class makes.
 *
 * If initialisation reported VS_FAIL (1) the actor is marked for destruction,
 * and either way the ActorBase implementation runs afterwards.
 *
 * Kept as a .c filename with the //cpp marker rather than renamed to .cpp: the
 * extension is what config/arm9/delinks.txt records, so renaming the file would
 * turn a src-only change into a config change too.
 */
#include "ActorDerived.h"

extern "C" void _ZN9ActorBase18AfterInitResourcesEj(ActorBase *self, u32 result);

void ActorDerived::AfterInitResources(u32 vfSuccess)
{
    if (vfSuccess == 1)
        MarkForDestruction();
    _ZN9ActorBase18AfterInitResourcesEj(this, vfSuccess);
}
