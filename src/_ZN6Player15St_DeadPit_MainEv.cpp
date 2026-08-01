//cpp
#include "types.h"
// @symbol _ZN6Player15St_DeadPit_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, int* pos, u32 d);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, int* pos);
extern void Vec3_RotateYAndTranslate(int* out, int* in, s16 angle, int* src);
extern void func_ov002_020c9718(u8* c);
extern void Player_AdvanceAnims(char* c);

extern u8 data_0209f2d8;
extern int data_ov002_021104fc[];
}

int Player::St_DeadPit_Main()
{
    switch (mStateStep) {
    case 0:
        break;
    case 1: {
        int b = (data_0209f2d8 == 1);
        if (b) {
            if (mStateWaitTimer == 0) {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021104fc);
                return 1;
            }
        }
        return 1;
    }
    case 2:
    case 5:
        if (mIsAirborne == 0) {
            mHorzSpeed = 0;
            mVertSpeed = 0;
        }
        if (mStateWork == 0) {
            if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
                mStateWork = 1;
                KillPlayer();
            }
        }
        break;
    case 3: {
        func_ov002_020c0108(((char*)this), 0);
        u32 snd = _ZN5Sound8PlayLongEjjjRK7Vector3j(mLoopingSoundHandle, 0, 0x10a, (int*)((char*)&mCamSpacePos), 0);
        int* p = (int*)(((long long)(int)((char*)&mSinkDepth)));
        mLoopingSoundHandle = snd;
        *p += 0x5000;
        if (mStateWork == 0) {
            if (mSinkDepth >= 0xb4000) {
                mStateWork = 1;
                KillPlayer();
            } else {
                *p += 0x2000;
                if (mStateArg == 0) {
                    if (mSinkDepth >= 0x64) {
                        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 0x25, (int*)((char*)&mCamSpacePos));
                        mStateArg = 1;
                    }
                }
            }
        }
        break;
    }
    case 6: {
        int src[3];
        src[0] = 0;
        src[1] = -0x1e000;
        src[2] = 0x64000;
        int* q = *(int**)((char*)&mAttachedActor);
        s16 angle = *(s16*)((char*)q + 0x8e);
        Vec3_RotateYAndTranslate((int*)((char*)&mPosX), (int*)((char*)q + 0x5c), angle, src);
        if (mStateTimer < 30) {
            mOpacity = 0;
        }
    }
    /* fallthrough */
    case 4:
        if (mStateTimer == 1) {
            KillPlayer();
            mOpacity = 0;
        }
        break;
    case 7:
    case 8:
        func_ov002_020c9718((u8*)((char*)this));
        if (mStateWork == 0) {
            if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
                mStateWork = 1;
                KillPlayer();
                mIsUnderwater = 1;
            }
        }
        break;
    case 9:
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
