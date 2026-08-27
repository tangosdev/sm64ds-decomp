//cpp
// @symbol _ZN14daObjUkiyuka_c8BehaviorEv
#include "daObjUkiyuka_c.h"
/* daObjUkiyuka_c::Behavior -- vtable slot 6, ov002 0x020b6494.
 *
 * Attributed by the vtable, not by a `recovered name:` comment -- the
 * pre-migration file carried none at all, only a `char *c` and raw offsets.
 * daObjUkiyuka_c's own vtable (ov002 0x0210912c) differs from dBgActor_c's in
 * slots 6, 9, 16 and 17; this address is slot 6, so it is this class's own
 * override and not either leaf's. include/daObjUkiyuka_c.h's banner already
 * names the four fields this body reads from exactly this function.
 *
 * THE FIELD SPELLINGS ARE THE PRE-MIGRATION ONES, deliberately. mBobPhase is
 * declared s16 but is stepped as a u16 and then sign-read as an s16 in the
 * same breath; mRestTimer is passed to DecIfAbove0_Short by address. Writing
 * either through the member rather than through the cast changes what mwccarm
 * CSEs -- see notes/mwccarm-codegen.md on compound assignment -- so the two
 * that matter keep the `(int)this + off` launder and the rest read as members.
 *
 * dBgActor_c::IsClsnInRange is NOT declared in include/dBgActor_c.h: its ROM name
 * carries by-value Fix12<int> parameters, which mwccarm passes differently
 * from the `int` this call site needs, so it stays an extern "C" of the
 * mangled symbol exactly as every other caller in the tree spells it. */

extern "C" {
extern s16 data_02082214[];
u16 DecIfAbove0_Short(u16 *p);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
}

s32 daObjUkiyuka_c::Behavior()
{
    char *c = (char *)this;

    if (DecIfAbove0_Short(&mRestTimer) != 0) {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
        return 1;
    }
    *(u16 *)((int)c + 0x328) += 0x100;
    {
        u16 h = (u16)((s64)mBobPhase);
        *(int *)((int)c + 0x60) -=
            (int)((((s64)mBobAmplitude * data_02082214[(h >> 4) * 2]) + 0x800) >> 0xc);
    }
    {
        int d = mPosY - mRestY;
        if (d < 0)
            d = -d;
        if (d == 0)
            mRestTimer = 0x3c;
    }
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
    return 1;
}
