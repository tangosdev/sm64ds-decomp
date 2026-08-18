//cpp
// @symbol _ZN17dScMgTrampoline_c16CleanupResourcesEv
#include "dScMgTrampoline_c.h"
/* dScMgTrampoline_c::CleanupResources -- vtable slot 3, ov006 0x021212e0.
 *
 * Named from the table: 0x021212e0 is the word slot 3 of
 * _ZTV17dScMgTrampoline_c holds where dScMgD3DBase_c's table holds something
 * else, so it is this class's own override of the virtual fBase_c declares. */

extern "C" void func_ov004_020ad90c(void *scene);

s32 dScMgTrampoline_c::CleanupResources()
{
    func_ov004_020ad90c(this);
    return 1;
}
