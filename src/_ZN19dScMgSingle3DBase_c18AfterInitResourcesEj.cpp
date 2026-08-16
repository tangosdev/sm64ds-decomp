//cpp
// @symbol _ZN19dScMgSingle3DBase_c18AfterInitResourcesEj
#include "decl_common.h"
#include "dScMgSingle3DBase_c.h"
/* dScMgSingle3DBase_c::AfterInitResources -- vtable slot 2.
 *
 * Attributed by the vtable, not by the old `recovered name:` comment, which said
 * dScMgFlower_c. That is one level too far down: dScMgFlower_c's parent IS this
 * class, and Flower's own overrides are different addresses entirely (slots 0/6/9
 * at 0x0212b480/0x0212ac74/0x0212aacc). 0x0210a6e4 is this class's own slot 2, and
 * the same vtable's slots 16/17 already carry _ZN19dScMgSingle3DBase_cD1Ev/D0Ev --
 * so the class attribution for this table was settled before this file.
 *
 * The pre-migration body chained with `_ZN11dScMgBase_c18AfterInitResourcesEj(c)`,
 * passing only `this`. The real signature takes vfSuccess in r1, and the ROM simply
 * passes its own r1 through, so naming the parameter and forwarding it is what the
 * cartridge already does. Qualified call, so this is a direct `bl` and not a
 * virtual dispatch. */

void dScMgSingle3DBase_c::AfterInitResources(u32 vfSuccess)
{
    dScMgBase_c::AfterInitResources(vfSuccess);
    mSysTracker.Initialise();
}
