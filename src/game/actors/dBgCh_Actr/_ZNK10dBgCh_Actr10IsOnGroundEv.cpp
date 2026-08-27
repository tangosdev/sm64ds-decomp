//cpp
// @symbol _ZNK10dBgCh_Actr10IsOnGroundEv
/* dBgCh_Actr::IsOnGround() const at 0x020356e8.
 *
 * Returns 0 or 0x10. Named like a predicate and shaped like a mask read:
 * `ldr r0,[r0,#0x10]; and r0,r0,#0x10; bx lr`, no normalisation, so bool
 * would be the wrong return type however the name reads.
 */
#include "dBgCh_Actr.h"

s32 dBgCh_Actr::IsOnGround() const
{
    return mFlags & 0x10;
}
