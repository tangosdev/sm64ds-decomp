//cpp
// @symbol _ZN6Player12St_Hurt_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Three-phase hurt state, sequenced by mStatePhase: 0 lands the hit (sound, voice,
 * knockback), 1 waits for horizontal speed to bleed off, 2 runs out the timer
 * and then hands back to the swimming or walking state depending on
 * mIsUnderwater. mStateWork is used as two nibbles here -- the high one
 * selects the hurt variant, the low one gates animation advance.
 */
#include "Player.h"
extern "C" {
extern void func_ov002_020d9a4c(void *c);
extern void func_ov002_020d99a4(unsigned char *self);
extern void func_ov002_020d9aac(void* c);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int a, void* v);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern void func_ov002_020d94cc(void *self);
extern void func_ov002_020bf9d4(void* c);
extern void func_ov002_020d98b4(void* self);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void Player_AdvanceAnims(void* c);
extern int func_ov002_020e2c84(void *self);

extern int data_ov002_0211067c;   /* the swim state */
extern int data_ov002_0211013c;   /* the walk state */
}

int Player::St_Hurt_Main()
{
    mIsBodyClsnEnabled = 1;
    if (mJumpComboTimer != 0) {
        mIsBodyClsnEnabled = 0;
    }

    switch (mStatePhase) {
    case 0:
        func_ov002_020d9a4c(this);
        func_ov002_020d99a4((unsigned char*)this);
        if (mIsAirborne != 0) break;
        func_ov002_020d9aac(this);
        mStateTimer = 0;
        if ((mStateStep & 7) >= 2) {
            _ZN5Sound9PlayBank0EjRK7Vector3((u32)mGroundSoundType + 0x50, &mCamSpacePosX);
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x11, &mCamSpacePosX);
        }
        if (mHurtDamage != 0) {
            u32 masked = mStateStep & 0xf0;
            if (masked == 0 || masked == 0x10) {
                func_ov002_020d94cc(this);
            }
            mHurtDamage = 0;
        }
        if (mIsInShallowWater == 0) {
            func_ov002_020bf9d4(this);
        }
        mStatePhase = 1;
        break;

    case 1:
        func_ov002_020d9a4c(this);
        func_ov002_020d99a4((unsigned char*)this);
        if (mIsAirborne != 0) break;
        func_ov002_020d9aac(this);
        if (mHorzSpeed != 0) break;
        mStatePhase = 2;
        if ((mStateWork >> 4) == 2) {
            mStateTimer = 0x5a;
        }
        break;

    case 2:
        if (mStateTimer != 0) {
            func_ov002_020d98b4(this);
            return 1;
        }
        mFlags &= ~0x80;
        mStateWork &= 0xf0;
        if (FinishedAnim()) {
            if ((mStateStep & 1) == 0) {
                mPrevAngleY = mPrevAngleY + 0x8000;
                mAngleY = mPrevAngleY;
            }
            mIsTakingDamage = 0;
            if (mStateArg & 2) {
                mInvincibleTimer = 0x24;
            }
            if (mIsUnderwater != 0) {
                _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211067c);
            } else {
                _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_0211013c);
            }
        }
        break;

    default:
        break;
    }

    if ((mStateWork & 0xf) == 0) {
        Player_AdvanceAnims(this);
    }
    mPrevVertSpeed = mVertSpeed;
    if (mIsAirborne == 0) {
        func_ov002_020e2c84(this);
    }
    return 1;
}
