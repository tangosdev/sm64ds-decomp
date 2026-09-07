//cpp
// @symbol _ZN7dScMB_c15graphCallback_c14GraphCallback2Ev
/* dScMB_c::graphCallback_c::GraphCallback2 -- slot 2, arm9 0x02034d24.
 *
 * Declines slot 2 the same way GraphCallback0 declines slot 0: the ROM body is
 * `mov r0,#0; bx lr`, against dGraph_c::callback_c's default of 1.
 */
#include "dScMB_c.h"

int dScMB_c::graphCallback_c::GraphCallback2()
{
    return 0;
}
