//cpp
#include "types.h"
// @symbol _ZN9ActorBase13AfterBehaviorEj
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"
/* ActorBase::AfterBehavior(u32 vfSuccess) at 0x02043af8 -- vtable slot 8.
 * Post-behavior hook; vfSuccess is the VirtualFuncSuccess code from Behavior().
 * Base ActorBase does nothing; leaf classes override.
 */
struct ActorBase;

void ActorBase::AfterBehavior(unsigned int vfSuccess_)
{
    u32 vfSuccess = (u32)vfSuccess_;
}
