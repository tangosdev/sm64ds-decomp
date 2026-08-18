//cpp
#include "types.h"
// @symbol _ZN9ActorBase11AfterRenderEj
/* recovered: named members + shared header, real C++ method */
#include "ActorBase.h"
/* ActorBase::AfterRender(u32 vfSuccess) at 0x02043ac4 -- vtable slot 11.
 * Post-render hook; vfSuccess is the VirtualFuncSuccess code from Render().
 * Base ActorBase does nothing; leaf classes override.
 */
struct ActorBase;

void ActorBase::AfterRender(unsigned int vfSuccess_)
{
    u32 vfSuccess = (u32)vfSuccess_;
}
