//cpp
// @symbol _ZN14dScMgD3DBase_c12BeforeRenderEv
/* recovered: real C++ method; class identity is preserved by compiler mangling */
#include "decl_Particle.h"
#include "dScMgD3DBase_c.h"

extern "C" int _ZN11dScMgBase_c12BeforeRenderEv(void *self);

int dScMgD3DBase_c::BeforeRender()
{
    if (_ZN11dScMgBase_c12BeforeRenderEv(this) == 0) {
        return 0;
    }

    _ZN8Particle9RenderAllEv();
    return 1;
}
