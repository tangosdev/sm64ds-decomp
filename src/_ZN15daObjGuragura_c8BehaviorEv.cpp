//cpp
// @symbol _ZN15daObjGuragura_c8BehaviorEv
#include "daObjGuragura_c.h"
/* daObjGuragura_c::Behavior -- vtable slot 6.
 *
 * Attributed by the vtable, not by a `recovered name:` comment: the old file
 * carried none, only the func_ov002_ address. The ROM's table at ov002 0x02109084
 * holds 0x020b616c in slot 6, which is what include/daObjGuragura_c.h's banner
 * already records -- it cites this very function as the witness that 0x320 and
 * 0x330 are two quaternions.
 *
 * The fields are spelled by name now that the receiver is typed: mTilt is the live
 * orientation, mTiltTarget what it lerps toward, mSettleDelay the countdown that
 * gates reloading the target, and mBumped the per-frame flag the body clears on the
 * way out. dActor_c::mFlags bit 3 is the "suppressed" bit -- while it is set the class
 * disables its collider and does nothing else.
 *
 * UpdateClsnPosAndRot is dBgActor_c's own non-virtual member, so it is a call now
 * rather than an `extern "C"` declaration of the mangled symbol. IsClsnInRange stays
 * mangled: include/dBgActor_c.h does not declare it, and that header belongs to the
 * whole dBgActor_c family rather than to this slice. dBgW's two members
 * stay mangled for the same reason.
 *
 * Was a C99 file. The `(int)((expr) != 0) != 0` idiom around the mFlags test is kept
 * verbatim rather than simplified: in C++ the inner `!= 0` is a bool and the width
 * of the cast is what steers the compare (see notes on the C++ bool widening cast),
 * so rewriting it is a byte-level change, not a tidy-up. */

extern "C" {
int  _ZN4dBgW9IsEnabledEv(void *thiz);
void _ZN4dBgW7DisableEv(void *thiz);
int  _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *thiz, int x, int z);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void Quaternion_SLerp(char *out, char *a, int t, char *b);
void func_ov002_020b6074(char *c);
extern int data_02092768[4];
}

s32 daObjGuragura_c::Behavior()
{
    char *c = (char *)this;

    if ((int)((mFlags & 8) != 0) != 0) {
        if (_ZN4dBgW9IsEnabledEv(c + 0x124)) {
            _ZN4dBgW7DisableEv(c + 0x124);
        }
        return 1;
    }
    if (DecIfAbove0_Byte(&mSettleDelay) == 0) {
        mTiltTarget[0] = data_02092768[0];
        mTiltTarget[1] = data_02092768[1];
        mTiltTarget[2] = data_02092768[2];
        mTiltTarget[3] = data_02092768[3];
    }
    Quaternion_SLerp((char *)mTilt, (char *)mTiltTarget, 0x199, (char *)mTilt);
    func_ov002_020b6074(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0)) {
        UpdateClsnPosAndRot();
    }
    mBumped = 0;
    return 1;
}
