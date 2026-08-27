//cpp
// @symbol _ZN7dScMB_c15graphCallback_c14GraphCallback0Ev
/* dScMB_c::graphCallback_c::GraphCallback0 -- slot 0, arm9 0x02034d2c.
 *
 * The Multi-Boot scene's graph callback declines slot 0: the ROM body is
 * `mov r0,#0; bx lr`. dGraph_c::callback_c's own default returns 1, so this
 * override exists purely to return 0 instead.
 *
 * Non-virtual by declaration -- see the note on the nested class in
 * include/dScMB_c.h.
 */
#include "dScMB_c.h"

int dScMB_c::graphCallback_c::GraphCallback0()
{
    return 0;
}
