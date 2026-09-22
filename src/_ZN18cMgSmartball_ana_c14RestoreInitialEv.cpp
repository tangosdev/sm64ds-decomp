//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then clears
 * the base's unk_031 flag, the low byte of the base's unk_032, and this
 * class's own countdown.
 *
 * Both go through the base's member names. That became possible when
 * cMgSmartball_board_c showed 0x031-0x033 to be three independent bytes
 * rather than a byte plus a halfword -- this class's byte-wide view now
 * agrees with the base's declaration, so no cast is needed. */
#include "cMgSmartball_ana_c.h"

void cMgSmartball_ana_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    unk_031 = 0;
    unk_032 = 0;
    mRespawnTimer = 0;
}
