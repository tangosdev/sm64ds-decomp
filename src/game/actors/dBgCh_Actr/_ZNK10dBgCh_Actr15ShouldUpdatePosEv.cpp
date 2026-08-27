//cpp
// @symbol _ZNK10dBgCh_Actr15ShouldUpdatePosEv
/* dBgCh_Actr::ShouldUpdatePos() const at 0x02035564.
 *
 * The inverse of the no-update-pos flag, and unlike the mask readers above
 * this one IS normalised: `ands r0,r0,#0x2000` then `moveq r0,#1` /
 * `movne r0,#0`, which is what inverting a flag costs.
 */
#include "dBgCh_Actr.h"

s32 dBgCh_Actr::ShouldUpdatePos() const
{
    return (mFlags & 0x2000) == 0;
}
