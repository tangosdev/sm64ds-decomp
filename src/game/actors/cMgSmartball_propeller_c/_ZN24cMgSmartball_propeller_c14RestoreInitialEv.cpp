//cpp
/* Slot 2. A real override, not a tail-call veneer: calls the base's
 * RestoreInitial() (qualified -- unqualified would dispatch through the
 * vtable and recurse), then clears the base's own angle at 0x32 (raw cast,
 * same reasoning as SaveSnapshot/Update -- see cMgSmartball_propeller_c.h),
 * resets the target rotation speed (mSpinSpeedTarget) to its default 0x40, zeroes
 * the current rotation speed (mSpinSpeed), and calls the shared helper that
 * recomputes the propeller-blade positions from the reset state. */
#include "cMgSmartball_propeller_c.h"

extern "C" void func_ov006_02110e28(void* p);

void cMgSmartball_propeller_c::RestoreInitial()
{
    cMgSmartball_object_c::RestoreInitial();
    *(short*)((char*)this + 0x32) = 0;
    mSpinSpeedTarget = 0x40;
    mSpinSpeed = 0;
    func_ov006_02110e28(this);
}
