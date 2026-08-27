//cpp
// @symbol _ZN6Player12St_Jump_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * Entering the jump state. Two guards get first refusal. Then the triple-jump
 * combo stage advances (mod 3) if the combo timer is still running, and is
 * reset by any of: sinking deeper than 0x1000, carrying something, being mega,
 * or -- at stage 2 without wings -- not running fast enough. Wings at stage 2
 * divert to a different state entirely; otherwise the per-stage launch speed
 * and animation are looked up by stage, speed is scaled by 0.8, quartered on a
 * sharp turn, and zeroed if a ledge check fires.
 */
#include "Player.h"
extern "C" {
extern int func_ov002_020e2be4(void *c);
extern int func_ov002_020e2b6c(void *c);
extern int Player_ScaleByCharFactor(void *c, int a);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *c, void *s);
extern void func_ov002_020bf2d8(void *c, int a);
extern void func_ov002_020e2ad0(void *c);
/* SetAnim takes a Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with a scalar in that slot. */
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *c, u32 a, int b, int fx, u32 d);
extern int AngleDiff(int a, int b);
extern int func_ov002_020c19d0(void *c, int a, int b);
extern void func_ov002_020e25f0(void *c, int a);
extern int data_ov002_0210a59c[];   /* launch speed, by combo stage */
extern int data_ov002_0210a5a8[];   /* jump animation, by combo stage */
extern char data_ov002_02110214;    /* the winged-jump state */
}

int Player::St_Jump_Init()
{
    int t;

    mJumpedFromQuicksand = 0;
    if (func_ov002_020e2be4(this) != 0)
        return 1;
    if (func_ov002_020e2b6c(this) != 0)
        return 1;

    mIsInAirState = 1;
    mIsFallScreaming = 0;
    if (mJumpComboTimer == 0) {
        mJumpComboStage = 0;
    } else {
        mJumpComboStage = (mJumpComboStage + 1) % 3;
    }

    if (mSinkDepth > 0x1000)
        goto clear;
    t = (mHeldObj != 0);
    if (t == false)
        goto skipclear;
clear:
    mJumpComboStage = 0;
skipclear:

    if (mIsMega != 0)
        mJumpComboStage = 0;

    if (mJumpComboStage == 2 && mHasWings == 0) {
        if (mHorzSpeed <= Player_ScaleByCharFactor(this, 0x14000))
            mJumpComboStage = 0;
    }

    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    if (mHasWings == 1 && mJumpComboStage == 2) {
        mStateStep = 0;
        _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_02110214);
    } else {
        int v = data_ov002_0210a59c[mJumpComboStage] + (mHorzSpeed >> 2);
        if (mIsMega != 0) {
            func_ov002_020bf2d8(this, (int)(((long long)v * 0x1400 + 0x800) >> 12));
        } else {
            func_ov002_020bf2d8(this, v);
        }
        func_ov002_020e2ad0(this);
        if (mIsMega == 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(this, data_ov002_0210a5a8[mJumpComboStage], 0x40000000, 0x1000, 0);
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x9c, 0x40000000, 0x1000, 0);
        }
        if (mHorzSpeed > Player_ScaleByCharFactor(this, 0x64000)) {
            mHorzSpeed = Player_ScaleByCharFactor(this, 0x64000);
        }
    }

    mStateWork = 0;
    if (param1 == 3) {
        if (mJumpComboStage != 2)
            mStateWork = 1;
    }

    mStateStep = 0;
    mStateArg = 0;
    unk_6cc = 0;
    func_ov002_020e25f0(this, mJumpComboStage);

    mHorzSpeed = (int)(((long long)mHorzSpeed * 0xccc + 0x800) >> 12);

    if (AngleDiff(mPrevAngleY, mAngleY) >= 0x4000) {
        mHorzSpeed >>= 2;
    }

    if (func_ov002_020c19d0(this, 0x64, 0x32) != 0) {
        mStateFlags |= 0x200;
        mHorzSpeed = 0;
    }
    return 1;
}
