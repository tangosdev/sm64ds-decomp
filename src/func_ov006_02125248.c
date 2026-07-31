// @symbol func_ov006_02125248
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgBSC_c.h"
// @emits dScMgBSC_c_OnGroundPounded
/* recovered: renamed to Class_Method */
/* dScMgBSC_c::OnGroundPounded - recovered from vtable slot identity */

int dScMgBSC_c_OnGroundPounded(void *this) {
    struct dScMgBSC_c *self = (struct dScMgBSC_c *)(void *)this;
    int x = self->unk_0b4;
    int v = x / 5;
    if ((unsigned int)v > 3) {
        v = 3;
    }
    return func_ov004_020b6324(v + 1);
}
