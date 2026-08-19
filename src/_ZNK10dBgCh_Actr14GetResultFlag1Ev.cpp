//cpp
// @symbol _ZNK12WithMeshClsn14GetResultFlag1Ev
/* WithMeshClsn::GetResultFlag1() const at 0x02035620.
 *
 * True when the sphere query found any collision at all. Reads mClsnFlags
 * with `ldrb` at 0x90 -- the result byte inside the SphereClsn sub-object,
 * not a field of our own -- and returns the masked bit, unnormalised.
 */
#include "WithMeshClsn.h"

s32 WithMeshClsn::GetResultFlag1() const
{
    return mClsnFlags & 0x1;
}
