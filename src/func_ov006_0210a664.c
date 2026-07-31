// @symbol func_ov006_0210a664
// @emits dScMgFlower_c_BeforeRender
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_Particle.h"
/* recovered: renamed to Class_Method */
/* dScMgFlower_c::BeforeRender - recovered from vtable slot identity */
typedef int Bool;

struct Scene;

extern Bool func_ov004_020b04f4(struct Scene* self);

Bool dScMgFlower_c_BeforeRender(struct Scene* self)
{
    if (!func_ov004_020b04f4(self))
        return 0;
    _ZN8Particle9RenderAllEv();
    return 1;
}
