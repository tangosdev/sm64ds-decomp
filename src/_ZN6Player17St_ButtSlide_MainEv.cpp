//cpp
// @symbol _ZN6Player17St_ButtSlide_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern void func_ov002_020bf90c(char* c);
extern void func_ov002_020c06fc(char* c, u32 a);
extern int func_ov002_020dd2f4(char* c);
extern int func_ov002_020c0688(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* s);
extern int func_0201226c(int a0, int a1, int a2, char* a3, int a4, int a5);
extern void func_ov002_020e25f0(char* c, int a);
extern void func_ov002_020c18b0(char* c, int a);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, char* v);
extern void func_ov002_020dc560(char* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char* c, u32 anim, int a, Fix12 b, u32 d);
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void Player_AdvanceAnims(char* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern int data_ov002_021101b4[];
extern int data_ov002_0211013c[];
}

int Player::St_ButtSlide_Main()
{
    switch (unk_6e6) {
    case 0:
        func_ov002_020bf90c(((char*)this));
        if (mIsAirborne == 0) {
            mIsSlidingOnGround = 1;
            func_ov002_020c06fc(((char*)this), 0x4000);
            func_ov002_020dd2f4(((char*)this));
        }
        if (mHorzSpeed == 0) {
            if (mSlideType == 0) {
                unk_6e6 = 2;
                mStateWork = 0;
                break;
            }
            (*(u8*)(((int)((char*)this) + 0x6e7)))++;
            if (mSlideStoppedTimer >= 0x1e) {
                unk_6e6 = 2;
                mStateWork = 0;
                break;
            }
        } else {
            mSlideStoppedTimer = 0;
        }
        if (func_ov002_020c0688(((char*)this)) != 0) {
            if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2) != 0
                && mStateWaitTimer == 0) {
                mJumpComboStage = 0;
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211019c);
                return 1;
            }
            mLoopingSoundHandle = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2, ((char*)this) + 0x74, mHorzSpeed, 0);
        } else {
            mStateStep = 0;
            unk_6e6 = 1;
            mStateWork = 0;
            if ((mStateArg | mSlideType) != 0)
                mVertSpeed = 0x15000;
            func_ov002_020e25f0(((char*)this), 0);
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            break;
        }
        func_ov002_020c18b0(((char*)this), 0);
        func_ov002_020dcafc(((char*)this));
        break;
    case 1:
        mAngZ = 0;
        mAngX = mAngZ;
        if (mIsAirborne == 0) {
            if (mLandSoundPlayed == 0) {
                mLandSoundPlayed = 1;
                _ZN5Sound9PlayBank0EjRK7Vector3(mGroundSoundType + 0x50, ((char*)this) + 0x74);
            }
            if (mStateWork == 0 && unk_558 >= 0xfc1) {
                mVertSpeed = -mPrevVertSpeed / 2;
                (*(u8*)(((int)((char*)this) + 0x6e5)))++;
            } else {
                unk_6e6 = 0;
                mVertSpeed = 0;
                break;
            }
        } else {
            func_ov002_020dc560(((char*)this));
        }
        (*(u8*)(((int)((char*)this) + 0x6e3)))++;
        if (mStateStep > 0x1e
            && mPosY - mGroundY > 0x1f4000) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101b4);
            return 1;
        }
        break;
    case 2:
        mAngZ = 0;
        mAngX = mAngZ;
        if (mStateWork == 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x42, 0x40000000, 0x1000, 0);
            mTargetAngleY = mAngleY;
            mStateWork = 1;
        } else if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        }
        break;
    }

    mPrevVertSpeed = mVertSpeed;
    Player_AdvanceAnims(((char*)this));
    mStateArg = mSlideType;
    return 1;
}
