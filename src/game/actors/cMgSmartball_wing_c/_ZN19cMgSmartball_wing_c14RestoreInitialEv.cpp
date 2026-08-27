//cpp
/* Slot 2. Calls the base's RestoreInitial (mCurrent = mInitial), then
 * zeroes the pad-region angle at 0x32 (see the header) and every unk_
 * field this class adds, in the ROM's own (non-address-order) sequence --
 * 0x34, then 0x3c, then 0x38, then 0x40, then 0x44. Tail-calls the shared
 * helper func_ov006_0210d93c, same as SaveSnapshot. */
#include "cMgSmartball_wing_c.h"

extern "C" void func_ov006_0210d93c(void *p);

void cMgSmartball_wing_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    *(unsigned short *)((char *)this + 0x32) = 0;
    unk_034 = 0;
    unk_03c = 0;
    unk_038 = 0;
    mTriggerCount = 0;
    mAngleSettled = 0;
    func_ov006_0210d93c(this);
}
