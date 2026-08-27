//cpp
// @symbol _ZN19dScMgSingle3DBase_c14BeforeBehaviorEv
#include "decl_common.h"
#include "dScMgSingle3DBase_c.h"
/* dScMgSingle3DBase_c::BeforeBehavior -- vtable slot 7.
 *
 * The particle tracker is only stepped when bit 0 of the global at 0x020a0db0 is
 * set, which is why the header's banner calls this the "updates it conditionally"
 * witness for mSysTracker at 0x471c. */

extern "C" {
extern unsigned int data_020a0db0;
}

int dScMgSingle3DBase_c::BeforeBehavior()
{
    if (dScMgBase_c::BeforeBehavior() == 0)
        return 0;
    if (data_020a0db0 & 1)
        mSysTracker.Update();
    return 1;
}
