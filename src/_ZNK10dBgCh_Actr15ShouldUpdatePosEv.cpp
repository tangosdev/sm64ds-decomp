//cpp
// @symbol _ZNK12WithMeshClsn15ShouldUpdatePosEv
/* WithMeshClsn::ShouldUpdatePos() const at 0x02035564.
 *
 * The inverse of the no-update-pos flag, and unlike the mask readers above
 * this one IS normalised: `ands r0,r0,#0x2000` then `moveq r0,#1` /
 * `movne r0,#0`, which is what inverting a flag costs.
 */
#include "WithMeshClsn.h"

s32 WithMeshClsn::ShouldUpdatePos() const
{
    return (mFlags & 0x2000) == 0;
}
