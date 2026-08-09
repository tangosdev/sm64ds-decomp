//cpp
// @symbol _ZNK12WithMeshClsn8IsOnWallEv
/* WithMeshClsn::IsOnWall() const at 0x0203562c.
 *
 * Returns 0 or 8, NOT a truth value: `ldrb r0,[r0,#0x90]; and r0,r0,#8;
 * bx lr` has no normalising `movne`, so the return type cannot be bool.
 */
#include "WithMeshClsn.h"

s32 WithMeshClsn::IsOnWall() const
{
    return mClsnFlags & 0x8;
}
