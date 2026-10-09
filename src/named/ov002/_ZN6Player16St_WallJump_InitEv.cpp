//cpp
// @symbol _ZN6Player16St_WallJump_InitEv
/* recovered: named members + shared header, real C++ method
 *
 * ov002, 0x020e17f8, size 0xb0. Entering the wall jump: clear the airborne
 * bookkeeping, start anim 0x28, set the launch speeds, TURN AROUND, and shout.
 *
 * The turn is the whole move. mAngleY += 0x8000 is exactly half a revolution
 * on a 16-bit angle, and mPrevAngleY is snapped to the new value in the same
 * breath so nothing interpolates through the 180 degrees -- the player faces
 * away from the wall instantly rather than spinning.
 *
 * Every offset the placeholder body carried is a named field of Player or of
 * its dActor_c base, so the shadow `struct Player` it used to declare is gone.
 * The vertical speed is chosen by param1: character 2 gets a fixed 0x2aa00,
 * everyone else goes through the ov002 worker with 0x3e000.
 *
 * `x = x + k` AND `x += k` ARE NOT THE SAME FUNCTION HERE, and that one line
 * is the whole difference between this file matching and not. On the s16
 * member the compound form makes mwcc materialise &mAngleY into a register --
 * `add r,r,#0x8e` then `ldrsh r,[r]` and later `strh r,[r]` -- one instruction
 * more than the ROM, which addresses the field directly off the object both
 * times (`ldrsh r,[r,#0x8e]` / `strh r,[r,#0x8e]`). Written out longhand it
 * matches. Nothing else in the function was ever wrong: the draft was 44 of 45
 * instructions correct and had been sitting unenrolled for it.
 */
#include "Player.h"

extern "C" {
extern void _ZN6Player7SetAnimEji5Fix12IiEj(Player *thiz, u32 a, int b, int c, u32 d);
extern void func_ov002_020bf2d8(void *c, int a);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, const void *v);
}

int Player::St_WallJump_Init()
{
    mJumpedFromQuicksand = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x28, 0x40000000, 0x1000, 0);
    if (param1 == 2) {
        mVertSpeed = 0x2aa00;
    } else {
        func_ov002_020bf2d8(this, 0x3e000);
    }
    mHorzSpeed = 0x18000;
    mAngleY = mAngleY + 0x8000;
    mPrevAngleY = mAngleY;
    mStateWork = 1;
    mStateStep = 0;
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xc, &mCamSpacePosX);
    return 1;
}
