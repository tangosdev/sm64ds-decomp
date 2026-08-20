#include "types.h"
// @symbol _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_dCc_c.h"
/* recovered: named members + shared header */
#include "dCcAc_c.h"
struct dActor_c { void* vtable; };


void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(struct dCcAc_c *self, struct dActor_c* actor, Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags) {
    self->owner = actor;
    _ZN5dCc_c4InitE5Fix12IiES1_jj(((void*)self), radius, height, flags, vulnFlags);
}