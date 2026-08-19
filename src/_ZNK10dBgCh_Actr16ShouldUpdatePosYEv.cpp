//cpp
// @symbol _ZNK12WithMeshClsn16ShouldUpdatePosYEv
/* WithMeshClsn::ShouldUpdatePosY() const at 0x02035578.
 *
 * As ShouldUpdatePos, for the Y axis alone -- same `ands`/`moveq`/`movne`
 * normalisation, mask 0x1000.
 */
#include "WithMeshClsn.h"

s32 WithMeshClsn::ShouldUpdatePosY() const
{
    return (mFlags & 0x1000) == 0;
}
