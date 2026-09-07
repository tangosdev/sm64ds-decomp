//cpp
#include "types.h"
// @symbol _ZN7fBase_c13AfterBehaviorEj
/* recovered: named members + shared header, real C++ method */
#include "fBase_c.h"
/* fBase_c::AfterBehavior(u32 vfSuccess) at 0x02043af8 -- vtable slot 8.
 * Post-behavior hook; vfSuccess is the VirtualFuncSuccess code from Behavior().
 * Base fBase_c does nothing; leaf classes override.
 */
struct fBase_c;

void fBase_c::AfterBehavior(unsigned int vfSuccess_)
{
    u32 vfSuccess = (u32)vfSuccess_;
}
