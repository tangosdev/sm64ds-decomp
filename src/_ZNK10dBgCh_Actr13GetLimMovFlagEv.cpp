//cpp
// @symbol _ZNK12WithMeshClsn13GetLimMovFlagEv
/* WithMeshClsn::GetLimMovFlag() const at 0x02035694.
 *
 * The limited-movement flag. Returns 0 or 0x80 -- the mask itself, not a
 * truth value; there is no normalisation between the `and` and the `bx`.
 */
#include "WithMeshClsn.h"

s32 WithMeshClsn::GetLimMovFlag() const
{
    return mFlags & 0x80;
}
