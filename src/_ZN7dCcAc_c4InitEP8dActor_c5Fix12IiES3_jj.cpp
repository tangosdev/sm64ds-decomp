//cpp
// @symbol _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj
/* recovered: named members + shared header */
#include "dCcAc_c.h"

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in dCcAc_c.h is the real one and callers may use it. */
extern "C" void _ZN5dCc_c4InitE5Fix12IiES1_jj(dCc_c *self, int radius, int height, u32 flags, u32 vulnFlags);

extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c *self, dActor_c *actor, int radius, int height, u32 flags, u32 vulnFlags)
{
    self->owner = actor;
    _ZN5dCc_c4InitE5Fix12IiES1_jj(self, radius, height, flags, vulnFlags);
}
