//cpp
// @symbol _ZN19dScMgSingle3DBase_c21AfterCleanupResourcesEj
#include "decl_common.h"
#include "dScMgSingle3DBase_c.h"
/* dScMgSingle3DBase_c::AfterCleanupResources -- vtable slot 5.
 *
 * The two volatile stores are the 3D engine's fog/clear registers (0x040004c8 and
 * 0x040004cc), written only on the teardown path where vfSuccess == 2. They stay
 * `volatile` on purpose: this is memory-mapped I/O, not codegen steering, even
 * though tiers.py's no_codegen_trick criterion counts the keyword either way. */

void dScMgSingle3DBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2) {
        *(volatile int *)0x40004c8 = 0x296a5800;
        *(volatile int *)0x40004cc = 0x7fff;
        CleanCommonModelDataArr();
    }
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}
