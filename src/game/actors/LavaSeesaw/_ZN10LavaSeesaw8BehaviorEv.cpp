//cpp
// @symbol _ZN10LavaSeesaw8BehaviorEv
#include "LavaSeesaw.h"

/* LavaSeesaw::Behavior -- vtable slot 6, ov022 0x02111dfc.
 *
 * Real member function: mSwingStep/mSwingCooldown are this class's own
 * named fields (include/LavaSeesaw.h); mAngleX is dActor_c's own field.
 * dBgActor_c::IsClsnInRangeE5Fix12IiES1_ stays raw extern "C" (bytes
 * refuse a real Fix12<int> parameter, dBgActor_c.h's own note);
 * UpdateClsnPosAndRot is a real method call. func_020393a4 and
 * func_ov022_02111d48 are this class's own, still-unmigrated helpers
 * (out of this task's scope), called under their existing names. */
extern "C" {
extern void func_020393a4(void *p, int v);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void func_ov022_02111d48(void *self);
}

int LavaSeesaw::Behavior()
{
    func_020393a4(&mMeshCollider, 0x650000);
    if (DecIfAbove0_Byte(&mSwingCooldown) == 0) {
        mAngleX += mSwingStep;
        if (mAngleX >= 0x400 || mAngleX <= -0x400) {
            mSwingStep = -mSwingStep;
            mSwingCooldown = 0x1e;
        }
    }
    func_ov022_02111d48(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0) {
        UpdateClsnPosAndRot();
    }
    return 1;
}
