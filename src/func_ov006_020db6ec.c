// @symbol func_ov006_020db6ec
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgCard_c.h"
// recovered name: dScMgCard_c_OnGroundPounded
/* recovered: renamed to Class_Method */
/* dScMgCard_c::OnGroundPounded - recovered from vtable slot identity */

int func_ov006_020db6ec(void *this) {
    struct dScMgCard_c *self = (struct dScMgCard_c *)(void *)this;
    int x = self->unk_0b4;
    int v;
    if (x < 5) {
        v = 1;
    } else if (x < 10) {
        v = 2;
    } else if (x < 15) {
        v = 3;
    } else {
        v = 4;
    }
    return func_ov004_020b6324(v);
}
