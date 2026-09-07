//cpp
// @symbol _ZN5dCc_cC2Ev
/* C2, the base-subobject constructor. The vptr store is the compiler's; the
 * body is only what the ROM's constructor actually writes -- both intrusive
 * list links zeroed, an unlinked node. */
#include "dCc_c.h"

dCc_c::dCc_c()
{
    prev = 0;
    next = 0;
}
