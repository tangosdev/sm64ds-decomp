//cpp
#include "types.h"
// @symbol _ZN6Player15St_DeadPit_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern u8 data_0209f2d8;
extern u8 data_0209f250;

int Player_ReleaseHeldActor(char* c);
void func_ov002_020d5cec(char* c);
void Player_DisableInteraction(char* c);
void HitDeathPlane(int arg);
void FUN_020299f4(void);
void _ZN6Player7SetAnimEji5Fix12IiEj(char* c, unsigned int a, int b, int f, unsigned int g);
void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, char* v);
}

int Player::St_DeadPit_Init()
{
    Player_ReleaseHeldActor(((char*)this));
    if (*(char**)((char*)&mObjInMouth) != 0) {
        int b = (*(u16*)(*(char**)((char*)&mObjInMouth) + 0xc) == 0xbf);
        if (b) {
            func_ov002_020d5cec(*(char**)((char*)&mObjInMouth));
            mStateFlags &= ~2;
            *(int*)(*(char**)((char*)&mObjInMouth) + 0xd0) = 0;
        }
        mUseAltBodyModel = 0;
        *(int*)(((int)*(char**)((char*)&mObjInMouth) + 0xb0)) |= 0x80000;
        *(int*)(((int)*(char**)((char*)&mObjInMouth) + 0xb0)) &= ~0x40000;
        *(int*)(((int)*(char**)((char*)&mObjInMouth) + 0xb0)) &= ~0x20000;
        *(char**)((char*)&mObjInMouth) = 0;
    }
    Player_DisableInteraction(((char*)this));
    {
        int t = 0;
        mIsBodyClsnEnabled = t;
        mStateWork = t;
        switch (mStateStep) {
        case 0:
            break;
        case 1:
            if (data_0209f2d8 == 1) t = 1;
            if (t == 0) {
                int a = 2;
                if (mSurfaceType == 5) a = 0;
                HitDeathPlane(a);
            } else {
                if (mPlayerNo == data_0209f250) FUN_020299f4();
                mStateWaitTimer = 0x1e;
            }
            unk_716 = 1;
            break;
        case 2:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 7, 0x40000000, 0x1000, t);
            mVertSpeed = 0xa000;
            mHorzSpeed >>= 1;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xb, ((char*)this) + 0x74);
            break;
        case 3:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x82, 0x40000000, 0x1000, t);
            mVertSpeed = 0;
            mHorzSpeed = 0;
            mStateArg = 0;
            break;
        case 4:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 2, 0x40000000, 0x1000, t);
            mStateTimer = 0x64;
            mVertSpeed = 0;
            mHorzSpeed = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xb, ((char*)this) + 0x74);
            break;
        case 5:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 6, 0x40000000, 0x1000, t);
            mVertSpeed = 0;
            mHorzSpeed = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0xb, ((char*)this) + 0x74);
            break;
        case 6:
            mStateTimer = 0x3c;
            mVertSpeed = t;
            mHorzSpeed = t;
            mVertAccel = t;
            break;
        case 7:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x84, 0x40000000, 0x1000, t);
            mVertAccel = 0;
            mTerminalVelocity = -0x1800;
            mVertSpeed = 0;
            mHorzSpeed = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x34, ((char*)this) + 0x74);
            break;
        case 8:
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x83, 0x40000000, 0x1000, t);
            mVertAccel = 0;
            mTerminalVelocity = -0x1800;
            break;
        case 9:
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x36, ((char*)this) + 0x74);
            break;
        }
    }
    return 1;
}
