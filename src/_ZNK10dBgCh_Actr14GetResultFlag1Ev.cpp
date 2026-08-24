//cpp
// @symbol _ZNK10dBgCh_Actr14GetResultFlag1Ev
/* dBgCh_Actr::GetResultFlag1() const at 0x02035620.
 *
 * True when the sphere query found any collision at all. Reads mClsnFlags
 * with `ldrb` at 0x90 -- the result byte inside the dBgCh_SphCrr sub-object,
 * not a field of our own -- and returns the masked bit, unnormalised.
 */
#include "dBgCh_Actr.h"

s32 dBgCh_Actr::GetResultFlag1() const
{
    return mSphereClsn.flags & 0x1;
}
