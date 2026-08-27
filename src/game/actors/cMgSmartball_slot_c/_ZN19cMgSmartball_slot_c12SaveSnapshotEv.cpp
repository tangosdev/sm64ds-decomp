//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family (the
 * base's copy is out-of-line and there is no bl here). Then runs three
 * independent countdowns: mSpinCooldown always decrements while positive;
 * mSpinsQueued decrements and fires the spin-start helper func_ov006_0210fb04
 * once mIsSpinning is clear and mSpinCooldown has drained; mSpinTimer decrements and
 * fires func_ov006_0210fa6c when it reaches exactly 0. Always tail-calls
 * the shared per-frame helper func_ov006_0210fb58. */
#include "cMgSmartball_slot_c.h"

extern "C" void func_ov006_0210fb04(void *self);
extern "C" void func_ov006_0210fa6c(void *self);
extern "C" void func_ov006_0210fb58(void *self);

void cMgSmartball_slot_c::SaveSnapshot()
{
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (mSpinCooldown > 0) {
        mSpinCooldown -= 1;
    }
    if (mIsSpinning == 0 && mSpinCooldown <= 0 && mSpinsQueued > 0) {
        mSpinsQueued -= 1;
        func_ov006_0210fb04(this);
    }
    if (mSpinTimer > 0) {
        mSpinTimer -= 1;
        if (mSpinTimer == 0)
            func_ov006_0210fa6c(this);
    }
    func_ov006_0210fb58(this);
}
