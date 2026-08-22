//cpp
// @symbol _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj
/* recovered: named members + shared header */
#include "dCcAcPos_c.h"

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az homes class-typed by-value parameters. The declaration in
   dCcAcPos_c.h is the real one and callers may use it. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c *self, dActor_c *actor, int radius, int height, u32 flags, u32 vulnFlags);

extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset, int radius, int height, u32 flags, u32 vulnFlags)
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(self, actor, radius, height, flags, vulnFlags);
    self->SetPosRelativeToActor(*offset);
}
