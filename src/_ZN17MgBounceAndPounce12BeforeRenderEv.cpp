//cpp
// @symbol _ZN17MgBounceAndPounce12BeforeRenderEv
/* recovered: real C++ method; class identity is preserved by compiler mangling */
#include "decl_Particle.h"
#include "MgBounceAndPounce.h"

extern "C" int _ZN11dScMgBase_c12BeforeRenderEv(void *self);

int MgBounceAndPounce::BeforeRender()
{
    if (_ZN11dScMgBase_c12BeforeRenderEv(this) == 0) {
        return 0;
    }

    _ZN8Particle9RenderAllEv();
    return 1;
}
