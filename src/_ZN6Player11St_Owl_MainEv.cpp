//cpp
// @symbol _ZN6Player11St_Owl_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* state);
extern void _Z14ApproachLinearRiii(int* p, int value, int speed);
extern int ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern void func_0201f32c(int a);
extern void func_0200d3f8(void* thiz, unsigned char playerID, void* ptr);
extern void Player_DisableInteraction(char* c);
extern void func_ov002_020c9e18(char* c);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int d, unsigned int e);
extern void Player_AdvanceAnims(void* c);

extern unsigned char data_020a0e40;
extern unsigned char data_0209f49c[];
extern char data_0209f4a0[];
extern int data_ov002_021101b4[];
extern short data_02082214[];
extern char* data_0209f318;
extern unsigned char data_0209d660;
}

int Player::St_Owl_Main()
{
    unsigned char st;

    mPeakY = mPosY;
    st = mStateStep;
    if (st != 2 && st != 3) {
        if (mHeldObj == 0 ||
            (*(unsigned short*)(data_0209f49c + data_020a0e40*0x18) & 2) == 0 ||
            (mClsnFlags & 2)) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101b4);
            return 1;
        }
    }

    switch (st) {
    case 0:
        if (mAttachOffsetY <= mPosY) {
            (*(unsigned char*)(((int)((char*)this) + 0x6e3)))++;
            mVertAccel = -0x2c00;
        }
        if (mIsAirborne != 0) {
            if (mVertSpeed < 0) {
                _Z14ApproachLinearRiii((int*)((char*)&mVertSpeed), 0x3c000, 0x8000);
            } else {
                _Z14ApproachLinearRiii((int*)((char*)&mVertSpeed), 0x3c000, 0x2000);
            }
        } else {
            mVertSpeed = 0x8000;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        ApproachAngle((short*)((char*)&mAngleY), mAngleYSpeed, 0x10, 0x100, 0);
        break;

    case 1:
    case 4: {
        int x = mHeldObj;
        short ang;
        int i;
        short mag = 10;
        x = *(int*)(x + 0x364);
        x = (unsigned short)((unsigned int)x >> 12);
        x += 0xb;
        ang = x * 0xccc;
        i = (unsigned short)ang >> 4;
        mVertSpeed = data_02082214[i*2+1] * mag;
        mHorzSpeed = 0x14000;
        if (*(short*)(data_0209f4a0 + data_020a0e40*0x18) != 0) {
            ApproachAngle((short*)((char*)&mAngleY), mDesiredAngleY, 0x20, 0x100, 0);
        }
        if (mStateStep == 1) {
            if (mAttachOffsetY - 0xed8000 > mPosY) {
                unk_724 = 1;
                func_0201f32c(0xa3);
                func_0200d3f8(data_0209f318, mPlayerNo, 0);
                mVertAccel = 0;
                Player_DisableInteraction(((char*)this));
                mStateStep = 2;
            }
        } else {
            if (mStateTimer == 0) {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101b4);
                return 1;
            }
        }
        break;
    }

    case 2:
        mHorzSpeed = 0;
        mVertSpeed = mHorzSpeed;
        if (data_0209d660 == 0) {
            mStateStep = 3;
            func_0200d6b4(data_0209f318, mPlayerNo);
        }
        break;

    case 3:
        if ((*(int*)(data_0209f318 + 0x154) & 0x8000) == 0) {
            mStateStep = 4;
            mStateTimer = 0x3c;
            func_ov002_020c9e18(((char*)this));
            unk_724 = 0;
        }
        break;
    }

    if (_ZN6Player12FinishedAnimEv(((char*)this))) {
        _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x58, 0, 0x1000, 0);
    }
    mTargetAngleY = mAngleY;
    Player_AdvanceAnims(((char*)this));
    return 1;
}
