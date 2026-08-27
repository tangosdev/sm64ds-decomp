//cpp
// @symbol _ZN7daDkk_c16OnAimedAtWithEggEv
/* func_ov025_0211199c, 8 bytes -- vtable slot 29, override of
 * dActor_c::OnAimedAtWithEgg (include/dActor_c.h, same signature). `mov
 * r0,#0xce000; bx lr`. 843776 == 0xce000 == a Fix12i egg auto-aim lock-on
 * radius of 206.0 -- wider than dActor_c's own default of 20.0
 * (_ZN8dActor_c16OnAimedAtWithEggEv, 0x14000).
 */
#include "daDkk_c.h"

int daDkk_c::OnAimedAtWithEgg()
{
    return 0xce000;
}
