//cpp
// @symbol _ZNK10dBgCh_Actr8IsOnWallEv
/* dBgCh_Actr::IsOnWall() const at 0x0203562c.
 *
 * Returns 0 or 8, NOT a truth value: `ldrb r0,[r0,#0x90]; and r0,r0,#8;
 * bx lr` has no normalising `movne`, so the return type cannot be bool.
 */
#include "dBgCh_Actr.h"

s32 dBgCh_Actr::IsOnWall() const
{
    return mSphereClsn.flags & 0x8;
}
