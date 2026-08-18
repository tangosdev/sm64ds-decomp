//cpp
// @symbol _ZN14dScMgMCarlo2_c16CleanupResourcesEv
#include "dScMgMCarlo2_c.h"
/* dScMgMCarlo2_c::CleanupResources -- vtable slot 3, ov006 0x020f9fe0.
 *
 * Attributed by the vtable: dScMgMCarlo2_c's own table is ov006 0x0213d7e8 and
 * its slot 3 relocates here, so this is this class's own override and not one
 * it inherits from dScMgSingle3DBase_c. The signature is include/fBase_c.h's
 * own slot 3, `virtual s32 CleanupResources()`.
 *
 * func_ov004_020ad90c IS ARITY-AMBIGUOUS ACROSS THE FAMILY, and this file does
 * not settle it: dScMgJump2_c's and dScMgTrampoline2_c's CleanupResources both
 * call it with no argument at all, this one passes the scene. `this` is
 * already in r0 at the call, so both spellings produce the same word and
 * neither is evidence. Kept as the pre-migration file had it. */

extern "C" void func_ov004_020ad90c(void *t);

s32 dScMgMCarlo2_c::CleanupResources()
{
    func_ov004_020ad90c(this);
    return 1;
}
