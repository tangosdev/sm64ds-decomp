//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same pattern as every sibling in this family. Then
 * decrements this class's own countdown (unk_034) while it is positive.
 * While the base's unk_031 flag is set, calls the out-of-scope helper
 * func_ov006_0210e120 (an escape/proximity check over the actor list): if
 * it reports clear and the countdown has drained to zero, unk_031 is
 * cleared; otherwise the countdown is reloaded to 0xa. */
#include "cMgSmartball_pushswitch_c.h"

extern "C" int func_ov006_0210e120(int *self);

void cMgSmartball_pushswitch_c::SaveSnapshot()
{
    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    if (unk_034 > 0) {
        unk_034 -= 1;
    }
    if (unk_031 != 1) return;
    if (func_ov006_0210e120((int *)this)) {
        if (unk_034 == 0) unk_031 = 0;
    } else {
        unk_034 = 0xa;
    }
}
