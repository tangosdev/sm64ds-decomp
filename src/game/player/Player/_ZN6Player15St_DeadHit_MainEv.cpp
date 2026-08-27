//cpp
#include "types.h"
// @symbol _ZN6Player15St_DeadHit_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020d94cc(char *c);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void *v);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void *v);
extern void _Z14ApproachLinearRiii(int *v, int a, int b);
extern u32 _ZNK6Player14GetBodyModelIDEjb(char *c, u32 a, bool b);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char *c, u32 a, int b, int d, u32 e);
extern int _ZN6Player12FinishedAnimEv(char *c);
extern void Player_AdvanceAnims(char *c);

extern u8 data_ov002_0211117c;
}

int Player::St_DeadHit_Main()
{
    u32 t;

    if (mIsAirborne == 0) {
        if (mStateArg == 0) {
            if (mPrevVertSpeed < 0) {
                mVertSpeed = -mPrevVertSpeed / 3;
                if (mVertSpeed <= 0x1000)
                    mVertSpeed = 0;
            }
            t = mStateStep & 0xf0;
            if (t == 0 || t == 0x10)
                func_ov002_020d94cc(((char *)this));
            _ZN5Sound9PlayBank0EjRK7Vector3(mGroundSoundType + 0x50, ((char *)this) + 0x74);
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x11, ((char *)this) + 0x74);
            mStateArg = 1;
        }
        _Z14ApproachLinearRiii((int *)((char *)&mHorzSpeed), 0, 0x1800);
    }

    switch (mStateWork) {
    case 0:
        if (_ZNK9Animation12WillHitFrameEi(
                (void *)((char *)mBodyModels[_ZNK6Player14GetBodyModelIDEjb(((char *)this), param1 & 0xff, 0)] + 0x50),
                data_ov002_02109db8[mStateStep & 1]) != 0)
            mStateWork = 1;
        break;
    case 1:
        if (mIsAirborne != 0) break;
        if (mStateArg == 0) break;
        if (mHorzSpeed != 0) break;
        _ZN6Player7SetAnimEji5Fix12IiEj(((char *)this), data_ov002_0210a07c[mStateStep & 1], 0x40000000, 0x1000, 0);
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xb, ((char *)this) + 0x74);
        mStateWork = 2;
        break;
    case 2:
        if (_ZN6Player12FinishedAnimEv(((char *)this)) != 0) {
            if (data_ov002_0211117c == 0) {
                KillPlayer();
                mStateWork = 3;
            }
        }
        break;
    }

    if (mStateWork != 1)
        Player_AdvanceAnims(((char *)this));
    mPrevVertSpeed = mVertSpeed;
    return 1;
}
