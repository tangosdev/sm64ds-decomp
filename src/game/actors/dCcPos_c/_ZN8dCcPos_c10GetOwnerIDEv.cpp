//cpp
#include "types.h"
// @symbol _ZN8dCcPos_c10GetOwnerIDEv
/* recovered: named members + shared header, real C++ method */
#include "dCcPos_c.h"
/* dCcPos_c::GetOwnerID() at 0x02014818
 * vtable slot 3. A positional cylinder has no owning dActor_c, so its owner ID
 * is always 0.
 */
struct dCcPos_c;

u32 dCcPos_c::GetOwnerID()
{
    (void)this;
    return 0;
}
