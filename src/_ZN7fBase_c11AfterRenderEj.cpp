//cpp
#include "types.h"
// @symbol _ZN7fBase_c11AfterRenderEj
/* recovered: named members + shared header, real C++ method */
#include "fBase_c.h"
/* fBase_c::AfterRender(u32 vfSuccess) at 0x02043ac4 -- vtable slot 11.
 * Post-render hook; vfSuccess is the VirtualFuncSuccess code from Render().
 * Base fBase_c does nothing; leaf classes override.
 */
struct fBase_c;

void fBase_c::AfterRender(unsigned int vfSuccess_)
{
    u32 vfSuccess = (u32)vfSuccess_;
}
