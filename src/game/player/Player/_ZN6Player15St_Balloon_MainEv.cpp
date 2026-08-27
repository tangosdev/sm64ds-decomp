//cpp
// @symbol _ZN6Player15St_Balloon_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Balloon (wing-cap style) flight, per frame. Bails out to one of two states
 * when the balloon timer runs out or the global flag says so; otherwise the
 * A-button bit adds lift in one of two bands, a collision bit bounces the
 * player off a wall by reflecting mPrevAngleY, and the pitch/roll pair is
 * driven from the turn rate.
 */
#include "Player.h"
extern "C" {
extern void _Z14ApproachLinearRiii(s32 &, s32, s32);
extern void func_ov002_020de968(void *);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *, Player::State &);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32, const Vector3 &);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32, s32);
extern int AngleDiff(s32, s16);
extern void func_ov002_020de428(void *);
extern void ApproachAngle(void *, s16, s16, s16, s32);
extern s32 func_ov002_020bf224(void *, s32, s32);
extern void _Z15ApproachLinear2Rsss(s16 &, s16, s16);
extern void func_ov002_020de3d0(void *, s16 *, s16 *);
extern void _ZN9Animation7AdvanceEv(void *);

extern u8 data_020a0e40;
extern s16 data_0209f49e[];
extern s16 data_0209f4a0[];
extern s16 data_02082214[];
extern Player::State data_ov002_021101b4;
extern Player::State data_ov002_021105a4;
}

s32 Player::St_Balloon_Main()
{
    if (mStateArg != 0) {
        mStateArg--;
    }
    mPeakY = mPosY;
    mVertAccel = 0;
    mTerminalVelocity = -0xc000;
    _Z14ApproachLinearRiii(mVertSpeed, -0x6000, 0x800);

    if (mBalloonTimer == 0) {
        func_ov002_020de968(this);
        _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_021101b4);
        return 1;
    }

    u16 flags = *(u16 *)((char *)&data_0209f49e + data_020a0e40 * 0x18);
    if (flags & 0x400) {
        func_ov002_020de968(this);
        _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_021105a4);
        return 1;
    }

    if ((flags & 2) && mStateWaitTimer == 0) {
        if (mStateTimer == 0) {
            mVertSpeed += 0xc000;
            if (mVertSpeed < 0xc000)
                mVertSpeed = 0xc000;
            if (mVertSpeed > 0x10000)
                mVertSpeed = 0x10000;
        } else {
            mVertSpeed += 0x6000;
            if (mVertSpeed < 0x6000)
                mVertSpeed = 0x6000;
            if (mVertSpeed > 0xc000)
                mVertSpeed = 0xc000;
        }
        _ZN5Sound9PlayBank0EjRK7Vector3(0xbe, *(const Vector3 *)&mCamSpacePosX);
        mStateTimer = 8;
    }

    if (mClsnFlags & 2) {
        s16 ang = _ZN4cstd5atan2E5Fix12IiES1_(mWallNormalX, mWallNormalZ);
        s16 diff = AngleDiff(ang, mPrevAngleY);
        if ((s32)diff > 0x4000) {
            func_ov002_020de428(this);
            mPrevAngleY = (s16)((ang * 2 + 0x8000) - mPrevAngleY);
            mHorzSpeed = (s32)(((long long)mHorzSpeed * 0x1400 + 0x800) >> 12);
            if (mHorzSpeed > 0x14000)
                mHorzSpeed = 0x14000;
            if (mHorzSpeed < 0x4000)
                mHorzSpeed = 0x4000;
            s32 idx = ((s32)(u16)diff >> 4) * 2;
            s16 t0 = data_02082214[idx + 1];
            s32 v1 = (s32)(((long long)mHorzSpeed * t0 + 0x800) >> 12);
            s32 v = (s32)(((long long)v1 * 0xa0 + 0x800) >> 12);
            s16 t1 = data_02082214[idx];
            mAngleYSpeed = (s16)(((long long)t1 * v + 0x800) >> 12);
            unk_69e = (s16)(((long long)t0 * v + 0x800) >> 12);
        }
    }

    u8 mode = mClsnFlags;
    if (mode & 1) {
        s32 g = mPrevVertSpeed;
        if (g < 0) {
            mVertSpeed = -(s32)(((long long)g * 0x2800 + 0x800) >> 12);
            func_ov002_020de428(this);
        }
    } else if (mode & 4) {
        s32 g = mPrevVertSpeed;
        if (g > 0) {
            mVertSpeed = -(s32)((((long long)g << 14) + 0x800) >> 12);
            mStateWaitTimer = 0x1e;
            func_ov002_020de428(this);
        }
    }

    if (mVertSpeed >= 0xa000)
        mVertSpeed = 0xa000;

    s32 r4 = 0;
    if (*(s16 *)((char *)&data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        ApproachAngle(&mPrevAngleY, mDesiredAngleY, 0x20, 0x2000, 0x10);
        r4 = 0x10000;
    }
    _Z15ApproachLinear2Rsss(mAngleY, mPrevAngleY, 0x200);
    _Z14ApproachLinearRiii(mHorzSpeed, func_ov002_020bf224(this, r4, 0), 0x100);
    func_ov002_020de3d0(this, &mAngleZ, &mAngleYSpeed);
    func_ov002_020de3d0(this, &mAngleX, &unk_69e);
    mPrevVertSpeed = mVertSpeed;
    mModelAnim3.Advance();
    return 1;
}
