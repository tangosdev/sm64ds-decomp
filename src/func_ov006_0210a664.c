// @symbol func_ov006_0210a664
// recovered name: dScMgFlower_c_BeforeRender
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Particle.h"
/* recovered: renamed to Class_Method */
/* dScMgFlower_c::BeforeRender - recovered from vtable slot identity */
typedef int Bool;

struct Scene;

extern Bool _ZN11dScMgBase_c12BeforeRenderEv(struct Scene* self);

Bool func_ov006_0210a664(struct Scene* self)
{
    if (!_ZN11dScMgBase_c12BeforeRenderEv(self))
        return 0;
    _ZN8Particle9RenderAllEv();
    return 1;
}
