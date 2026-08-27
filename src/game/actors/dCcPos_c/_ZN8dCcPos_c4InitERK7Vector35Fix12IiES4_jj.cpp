//cpp
// @symbol _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj
/* recovered: named members + shared header */
#include "dCcPos_c.h"

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az homes class-typed by-value parameters. The declaration in
   dCcPos_c.h is the real one and callers may use it. */
extern "C" void _ZN5dCc_c4InitE5Fix12IiES1_jj(dCc_c *self, int radius, int height, u32 flags, u32 vulnFlags);

extern "C" void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(dCcPos_c *self, const Vector3 *pos, int radius, int height, u32 flags, u32 vulnFlags)
{
    self->pos.x = pos->x;
    self->pos.y = pos->y;
    self->pos.z = pos->z;
    _ZN5dCc_c4InitE5Fix12IiES1_jj(self, radius, height, flags, vulnFlags);
}
