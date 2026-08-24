//cpp
// @symbol _ZN15daObjRcCarpet_c8BehaviorEv
#include "daObjRcCarpet_c.h"

int ApproachLinear(int &value, int target, int step);
extern "C" {
extern void func_ov036_021122c0(daObjRcCarpet_c *self);
extern void func_ov036_0211224c(daObjRcCarpet_c *self);
/* The true signature carries two by-value Fix12<int> parameters, which mwccarm
   homes to the stack. Keep the verified scalar ABI spelling (runbook wall 6az). */
extern void _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* t, int a, int b);
}

int daObjRcCarpet_c::Behavior() {
    if (mAfterClsnRan != 0)
        ApproachLinear(unk_4bc, -0x14000, 0x5000);
    else
        ApproachLinear(unk_4bc, 0, 0x5000);
    BaseBehavior();
    mModelAnim.Advance();
    func_ov036_021122c0(this);
    func_ov036_0211224c(this);
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    return 1;
}
