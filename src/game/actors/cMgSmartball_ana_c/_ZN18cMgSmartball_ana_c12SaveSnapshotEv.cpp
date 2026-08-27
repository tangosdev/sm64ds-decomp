//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family (the
 * base's copy is out-of-line and there is no bl here). Then decrements this
 * class's own countdown (mRespawnTimer); when it reaches zero it calls the shared
 * helper func_ov006_02114ec0 and sets the BASE's unk_032 byte to 1 -- the
 * low byte only. 0x031-0x033 is a three-byte region that three children read
 * three incompatible ways; this class's byte-wide view is one of them. See
 * cMgSmartball_object_c.h. */
#include "cMgSmartball_ana_c.h"

extern "C" void func_ov006_02114ec0(void *self);

void cMgSmartball_ana_c::SaveSnapshot()
{
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mRespawnTimer <= 0)
        return;
    mRespawnTimer = mRespawnTimer - 1;
    if (mRespawnTimer > 0)
        return;
    func_ov006_02114ec0((void *)mpManager);
    unk_032 = 1;
}
