// @symbol func_ov006_0212aa74
// recovered name: dScMgFlower_c_OnYoshiTryEat_0212aa74
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgFlower_c::OnYoshiTryEat - recovered from vtable slot identity */
/* func_ov006_0212aa74 at 0x0212aa74
 *
 * Wraps a counter at this+0x5fe4 (increments while <= 0x14, else resets
 * to 0), then updates the sub-object at this+0x51f8 (func_ov006_020c3bc8)
 * and runs func_ov006_0212a764.
 *
 * Matching notes: the increment must be the THEN arm (fall-through) with
 * the reset as the small predicated else (movgt/strgt + bgt), and it is
 * spelled as a volatile RMW through a u64-mask-laundered pointer so the
 * counter address is rematerialized from the literal pool in its own
 * block instead of folding into the this+0x5000 base and if-converting.
 */

void func_ov006_0212aa74(char *self)
{
    if (*(int *)(self + 0x5fe4) <= 0x14) {
        (*(volatile int *)(self + 0x5fe4))++;
    } else {
        *(int *)(self + 0x5fe4) = 0;
    }
    func_ov006_020c3bc8(self + 0x51f8);
    func_ov006_0212a764(self);
}
