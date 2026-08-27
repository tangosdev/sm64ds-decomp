//cpp
// @symbol _ZN19dScMgSingle3DBase_c12BeforeRenderEv
#include "decl_common.h"
#include "dScMgSingle3DBase_c.h"
/* dScMgSingle3DBase_c::BeforeRender -- vtable slot 10.
 *
 * The pre-migration file declared a bare `struct dScene_c;` and took a `dScene_c *self`,
 * because the base call's mangling was all it had to go on. The real receiver is
 * this class, and Particle::RenderAll is a free function in that namespace rather
 * than anything hanging off dScene_c. */

int dScMgSingle3DBase_c::BeforeRender()
{
    if (!dScMgBase_c::BeforeRender())
        return 0;
    Particle::RenderAll();
    return 1;
}
