//cpp
// @symbol _ZNK12WithMeshClsn13JustHitGroundEv
/* WithMeshClsn::JustHitGround() const at 0x0203571c.
 *
 * Set for the single frame the actor lands. Returns 0 or 0x20, the mask
 * itself -- same unnormalised shape as IsOnGround.
 */
#include "WithMeshClsn.h"

s32 WithMeshClsn::JustHitGround() const
{
    return mFlags & 0x20;
}
