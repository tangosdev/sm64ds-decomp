//cpp
// @symbol _ZN9dScDSMT_c15graphCallback_c14GraphCallback0Ev
/* dScDSMT_c::graphCallback_c::GraphCallback0 -- slot 0, ov007 0x020cc110.
 *
 * Declines slot 0: the ROM body is `mov r0,#0; bx lr`, against
 * dGraph_c::callback_c's default of 1.
 */
#include "dScDSMT_c.h"

int dScDSMT_c::graphCallback_c::GraphCallback0()
{
    return 0;
}
