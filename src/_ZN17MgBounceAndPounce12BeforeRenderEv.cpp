//cpp
// @symbol _ZN17MgBounceAndPounce12BeforeRenderEv
/* recovered: real C++ method; class identity is preserved by compiler mangling */
#include "decl_Particle.h"
#include "MgBounceAndPounce.h"

extern "C" int func_ov004_020b04f4(void *self);

int MgBounceAndPounce::BeforeRender()
{
    if (func_ov004_020b04f4(this) == 0) {
        return 0;
    }

    _ZN8Particle9RenderAllEv();
    return 1;
}
