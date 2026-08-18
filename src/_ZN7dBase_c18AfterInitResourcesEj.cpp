//cpp
// @symbol _ZN7dBase_c18AfterInitResourcesEj
/* dBase_c::AfterInitResources(u32) at 0x02013ef4 -- vtable slot 2, the only
 * functional override this class makes.
 *
 * If initialisation reported VS_FAIL (1) the actor is marked for destruction,
 * and either way the fBase_c implementation runs afterwards.
 *
 * Kept as a .c filename with the //cpp marker rather than renamed to .cpp: the
 * extension is what config/arm9/delinks.txt records, so renaming the file would
 * turn a src-only change into a config change too.
 */
#include "dBase_c.h"

void dBase_c::AfterInitResources(u32 vfSuccess)
{
    if (vfSuccess == 1)
        MarkForDestruction();
    fBase_c::AfterInitResources(vfSuccess);
}
