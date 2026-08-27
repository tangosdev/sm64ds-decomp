//cpp
// @symbol _ZN8dActor_c22BeforeCleanupResourcesEv
/* dActor_c::BeforeCleanupResources() at 0x02011220 -- vtable slot 4.
 *
 * Normalises the fBase_c result to 0 or 1.
 *
 * Kept as a .c filename with the //cpp marker; see the note in
 * src/_ZN8dActor_c18AfterInitResourcesEj.c.
 */
#include "dActor_c.h"

int dActor_c::BeforeCleanupResources()
{
    if (fBase_c::BeforeCleanupResources() != 0)
        return 1;
    return 0;
}
