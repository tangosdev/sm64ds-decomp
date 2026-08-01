//cpp
#include "types.h"
// @symbol _ZN6Player12St_Bonk_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct Camera;

extern "C" {
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void* v);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void* v);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void Player_AdvanceAnims(void* c);
extern void func_0200d8c8(struct Camera* cam, const struct Vector3* v, int strength);

extern struct Camera* data_0209f318;
extern int data_ov002_0211013c[];
}

int Player::St_Bonk_Main()
{
    int rate;

    if (mStateTimer != 0) {
        mHorzSpeed = 0x40000;
        mVertSpeed = 0x8000;
        goto ret1;
    }

    rate = 0x1000;
    if (mIsAirborne == 0) {
        int v = mPrevVertSpeed;
        if (v < 0) {
            mVertSpeed = (-v) / 3;
            if (mVertSpeed <= 0x1000) {
                mVertSpeed = 0;
            }
        }

        if (mStateArg == 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(mGroundSoundType + 0x50, ((char*)this) + 0x74);
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x11, ((char*)this) + 0x74);
            mStateArg = 1;
        }

        if (mPeakY - mPosY > 0xbb8000) {
            func_0200d8c8(data_0209f318, (struct Vector3*)((char*)&mPosX), 0x7d0000);
        }

        rate = 0x1800;
    }

    _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), 0, rate);

    if (mHorzSpeed != 0) {
        goto willhit;
    }
    if (mIsAirborne == 0) {
        goto finishedanim;
    }
willhit:
    {
        u32 arg = (u8)mParam;
        int modelIdx = _ZNK6Player14GetBodyModelIDEjb(((char*)this), arg, 0);
        char* anim = *(char**)(((char*)this) + modelIdx * 4 + 0xdc) + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x2e) != 0) {
            mStateWork = 1;
        }
    }
    goto tail;
finishedanim:
    mStateWork = 0;
    if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
        mTargetAngleY = mTargetAngleY + 0x8000;
        mAngleY = mTargetAngleY;
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        return 1;
    }
tail:
    if (mStateWork == 0) {
        Player_AdvanceAnims(((char*)this));
    }
    mPrevVertSpeed = mVertSpeed;
ret1:
    return 1;
}
