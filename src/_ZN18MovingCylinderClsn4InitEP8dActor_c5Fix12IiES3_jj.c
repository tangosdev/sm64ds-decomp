#include "types.h"
// @symbol _ZN18MovingCylinderClsn4InitEP8dActor_c5Fix12IiES3_jj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_CylinderClsn.h"
/* recovered: named members + shared header */
#include "MovingCylinderClsn.h"
struct dActor_c { void* vtable; };


void _ZN18MovingCylinderClsn4InitEP8dActor_c5Fix12IiES3_jj(struct MovingCylinderClsn *self, struct dActor_c* actor, Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags) {
    self->owner = actor;
    _ZN12CylinderClsn4InitE5Fix12IiES1_jj(((void*)self), radius, height, flags, vulnFlags);
}