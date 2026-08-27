//cpp
#include "types.h"
// @symbol _ZN6Player14St_Thrown_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020bf90c(char* c);
extern void func_ov002_020c06fc(char* c, u32 mask);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, const Vector3& v);
extern void func_ov002_020bf9d4(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12i a, int b);
extern void _Z15ApproachLinear2Rsss(s16* v, s16 target, s16 step);
extern int _ZN6Player9GetHealthEv(void* c);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void Player_AdvanceAnims(char* c);

extern int data_ov002_0211031c[];
extern int data_ov002_0211010c[];
extern int data_ov002_0211013c[];
}

int Player::St_Thrown_Main()
{
    if (mStateTimer == 1) {
        *(int*)((int)((char*)this) + 0x2ec) |= 0x2000;
    }
    func_ov002_020bf90c(((char*)this));

    u8 state = mStateStep;
    switch (state) {
    case 0:
        if (mIsAirborne == 0) {
            mAngleX = 0;
            *(int*)((int)((char*)this) + 0x2ec) &= ~0x2000;
            func_ov002_020c06fc(((char*)this), 0x8000);
            if (mStateArg == 0) {
                mStateArg = 1;
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter, 6, *(Vector3*)((char*)&mCamSpacePosX));
            }
            int r5 = func_ov002_020e2c84(((char*)this));
            if (r5 != 2 && mIsInShallowWater == 0) {
                func_ov002_020bf9d4(((char*)this));
            }
            if (r5 != 0) {
                return 1;
            }
            if (mFloorNormalY == 0x1000) {
                if (mHorzSpeed == 0) {
                    mStateStep = 1;
                    goto L1f8;
                }
            } else {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211031c);
                return 1;
            }
        }
        mAngleY = mPrevAngleY;
        mHorzSpeed = (int)(((long long)mHorzSpeed * 0xfae + 0x800) >> 12);
        if (mIsAirborne != 0) {
            int a = _ZN4cstd5atan2E5Fix12IiES1_(mHorzSpeed, mVertSpeed);
            s16 b = a - 0x4000;
            if (b > 0x3000) b = 0x3000;
            mAngleX = b;
        } else {
            _Z15ApproachLinear2Rsss((s16*)((char*)&mAngleX), 0, 0x400);
        }
        break;
    case 1:
        if (_ZN6Player9GetHealthEv(((char*)this)) == 0) {
            mStateStep = 1;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211010c);
            return 1;
        }
        if (_ZN6Player12FinishedAnimEv(((char*)this)) != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            mPrevAngleY = mAngleY;
        }
        Player_AdvanceAnims(((char*)this));
        break;
    }

L1f8:
    if (mIsAirborne == 0) {
        func_ov002_020e2c84(((char*)this));
    }
    return 1;
}
