//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family (the
 * base's copy is out-of-line and there is no bl here). Then decrements this
 * class's own countdown (unk_034); when it reaches zero it calls the shared
 * helper func_ov006_02114ec0 and sets the BASE's unk_032 byte -- low byte
 * only, see cMgSmartball_ana_c.h and cMgSmartball_object_c.h for why that
 * field's width is contested -- to 1. */
#include "cMgSmartball_ana_c.h"

extern "C" void func_ov006_02114ec0(void *self);

void cMgSmartball_ana_c::SaveSnapshot()
{
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (unk_034 <= 0)
        return;
    unk_034 = unk_034 - 1;
    if (unk_034 > 0)
        return;
    func_ov006_02114ec0((void *)unk_004);
    *(u8 *)((char *)this + 0x32) = 1;
}
