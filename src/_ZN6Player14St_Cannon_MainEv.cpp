//cpp
#include "types.h"
// @symbol _ZN6Player14St_Cannon_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern short GetAngleToCamera(int i);
extern void func_0200d6f0(void* thiz, u8 pid);
extern void func_02012694(u32 id, void* v);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void* v);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void* v);
extern void func_ov002_020c9e18(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int cc);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void Player_AdvanceAnims(void* c);
extern int _ZN6Player12FinishedAnimEv(void* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern char* data_0209f318;
extern short data_02082214[];
extern int data_ov002_0211031c[];
extern int data_ov002_02110214[];
}

int Player::St_Cannon_Main()
{
    switch (mStateStep) {
    case 0:
        break;
    case 1: {
        if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 3) == 0)
            break;

        char* p = data_0209f318;
        mPrevAngleY = GetAngleToCamera(mPlayerNo) + 0x8000;
        mAngleY = mPrevAngleY;

        int ang = *(u16*)(p + 0x17e);
        int j = ang >> 4;
        mHorzSpeed = (int)(((long long)data_02082214[j * 2 + 1] * 0x64000 + 0x800) >> 12);
        mVertSpeed = (int)(((long long)data_02082214[j * 2] * 0x64000 + 0x800) >> 12);
        mAngleX = 0;
        mStateStep = 2;
        mOpacity = 0x1f;
        mIsBodyClsnEnabled = 1;
        *(int*)(((long long)(int)((char*)&mBodyClsnFlags))) |= 0x20;
        func_0200d6f0(p, mPlayerNo);
        func_02012694(0x14f, ((char*)this) + 0x74);
        _ZN5Sound9PlayBank0EjRK7Vector3(0xb9, ((char*)this) + 0x74);
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xc, ((char*)this) + 0x74);
        func_ov002_020c9e18(((char*)this));
        mIsControlDisabled = 0;
        break;
    }
    case 2:
        if ((mClsnFlags & 6) != 0) {
            func_ov002_020c1eb4(((char*)this), (short)(mAngleY + 0x8000));
            return 1;
        }
        if (mIsAirborne == 0) {
            if (mStateWork != 0) {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211031c);
                return 1;
            }
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211031c);
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            mVertSpeed = 0xa000;
            return 1;
        }
        if (mHasWings != 0 && mVertSpeed < 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x72, 0x40000000, 0x1000, 0);
            mStateStep = 3;
            return 1;
        }
        *(int*)(((long long)(int)((char*)&mHorzSpeed))) -= 0x80;
        if (mHorzSpeed < 0xa000)
            mHorzSpeed = 0xa000;
        if (mVertSpeed >= 0)
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(mPosX, mPosY, mPosZ);
        if (mStateWork == 0)
            mAngleX = _ZN4cstd5atan2E5Fix12IiES1_(mHorzSpeed >> 8, mVertSpeed >> 8) - 0x4000;
        Player_AdvanceAnims(((char*)this));
        // fall through
    case 3:
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            mStateStep = 1;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110214);
        }
        Player_AdvanceAnims(((char*)this));
        break;
    }
    return 1;
}
