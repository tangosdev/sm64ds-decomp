//cpp
// @symbol _ZN7daOts_c6RenderEv
/* recovered: named members + shared header, real C++ method -- vtable slot 9
 *
 * WAS _ZN5Bully6RenderEv, and misattributed the same way as CleanupResources: slot 9
 * holds 0x02116cf0 in daOts_c, Bully AND ChillBully. BigBully is the only one of the
 * three that overrides it (0x0211764c), which is exactly the pattern of an inherited
 * method with one child that replaces it.
 */
#include "daOts_c.h"

int daOts_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}
