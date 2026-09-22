//cpp
// @symbol _ZNK10dBgCh_Actr16ShouldUpdatePosYEv
/* dBgCh_Actr::ShouldUpdatePosY() const at 0x02035578.
 *
 * As ShouldUpdatePos, for the Y axis alone -- same `ands`/`moveq`/`movne`
 * normalisation, mask 0x1000.
 */
#include "dBgCh_Actr.h"

s32 dBgCh_Actr::ShouldUpdatePosY() const
{
    return (mFlags & 0x1000) == 0;
}
