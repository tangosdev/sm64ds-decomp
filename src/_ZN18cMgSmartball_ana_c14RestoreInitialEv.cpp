//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then clears
 * the base's unk_031 flag, the low byte of the base's unk_032, and this
 * class's own countdown.
 *
 * unk_031 goes through the base's member name -- same width, no ambiguity.
 * 0x32 stays a raw cast: the ROM stores ONE byte into a field the base
 * declares 16 bits wide, so the name would misstate the access. See
 * cMgSmartball_object_c.h on that field's contested width. */
#include "cMgSmartball_ana_c.h"

void cMgSmartball_ana_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    unk_031 = 0;
    *(u8 *)((char *)this + 0x32) = 0;
    unk_034 = 0;
}
