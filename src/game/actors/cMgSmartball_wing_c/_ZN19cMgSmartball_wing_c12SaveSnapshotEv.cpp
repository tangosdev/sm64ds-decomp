//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family (the
 * base's copy is out-of-line and there is no bl here). Then, unless mAngleSettled
 * is 1 or mTriggerCount is not positive, eases the angle at offset 0x32 (see the
 * header -- it belongs to the BASE, not this class) toward
 * 0x3000 by steps of 0x200, clamping on overshoot in either direction.
 * Always tail-calls the shared helper func_ov006_0210d93c, which touches
 * the unmodelled 0x48..0x87 tail (see the header). */
#include "cMgSmartball_wing_c.h"

extern "C" void func_ov006_0210d93c(void *self);

void cMgSmartball_wing_c::SaveSnapshot()
{
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mAngleSettled != 1 && mTriggerCount > 0) {
        if (*(s16 *)((char *)this + 0x32) < 0x3000) {
            (*(s16 *)((char *)this + 0x32)) += 0x200;
            if (*(s16 *)((char *)this + 0x32) >= 0x3000) *(s16 *)((char *)this + 0x32) = 0x3000;
        } else {
            (*(s16 *)((char *)this + 0x32)) -= 0x200;
            if (*(s16 *)((char *)this + 0x32) < 0x3000) *(s16 *)((char *)this + 0x32) = 0x3000;
        }
    }
    func_ov006_0210d93c(this);
}
