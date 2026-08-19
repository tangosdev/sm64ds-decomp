//cpp
// @symbol _ZNK10dBgCh_Actr13GetLimMovFlagEv
/* dBgCh_Actr::GetLimMovFlag() const at 0x02035694.
 *
 * The limited-movement flag. Returns 0 or 0x80 -- the mask itself, not a
 * truth value; there is no normalisation between the `and` and the `bx`.
 */
#include "dBgCh_Actr.h"

s32 dBgCh_Actr::GetLimMovFlag() const
{
    return mFlags & 0x80;
}
