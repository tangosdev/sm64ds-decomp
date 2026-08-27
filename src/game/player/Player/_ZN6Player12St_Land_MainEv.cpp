//cpp
#include "types.h"
// @symbol _ZN6Player12St_Land_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020c0434(void* c);
extern void func_ov002_020c0364(void* c, u32 arg);
extern void func_ov002_020c06fc(void* c, u32 arg);
extern int func_ov002_020e3078(void* c, void* s);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int _ZN6Player6IsAnimEj(void* c, u32 a);
extern int Player_ScaleByCharFactor(void* c, int a);
extern int func_ov002_020bf224(void* c, int a, int b);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern int _ZN6Player7IsStateERNS_5StateE(void* c, void* s);
extern void Player_AdvanceAnims(void* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern u16 data_0209f49c[];
extern s16 data_0209f4a0[];
extern int data_ov002_0211052c[];
extern int data_ov002_0211013c[];
}

int Player::St_Land_Main()
{
    if (func_ov002_020c0434(((char*)this))) {
        func_ov002_020c0364(((char*)this), 3);
        func_ov002_020c06fc(((char*)this), 0x4000);
        return 1;
    }

    func_ov002_020e04a4(((char*)this));
    u16 temp_r3 = mStateFlags;

    if ((u16)(temp_r3 & 0x40) == 0) {
        if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2)
            || (u16)(temp_r3 & 0x100) != 0) {
            *(u16*)((char*)&mStateFlags) &= ~0x100;
            if (func_ov002_020e3078(((char*)this), data_ov002_0211055c) != 0
                && (*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 0x400)
                && mHorzSpeed >= 0) {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211055c);
                return 1;
            }
            if (_ZN6Player6IsAnimEj(((char*)this), 0x4e) != 0) {
                mAngleY = mPrevAngleY;
            }
            int v = (mHeldObj != 0);
            if (v != 0) {
                mJumpComboTimer = 0;
            }
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211019c);
            return 1;
        }
        if (func_ov002_020d5c6c(((char*)this)) != 0) {
            return 1;
        }
        if (*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 1) {
            return func_ov002_020dde74(((char*)this));
        }
    } else {
        *(u16*)((char*)&mStateFlags) &= ~0x100;
    }

    _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed),
        func_ov002_020bf224(((char*)this), Player_ScaleByCharFactor(((char*)this), 0x20000), 0),
        0x4000);

    if (func_ov002_020e3078(((char*)this), data_ov002_0211052c) != 0) {
        mHorzSpeed = 0;
    }

    if (mStateTimer != 0) {
        goto tail;
    }

    if ((*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0)
        || (_ZN6Player12FinishedAnimEv(((char*)this)) != 0)) {
        if (_ZN6Player6IsAnimEj(((char*)this), 0x1b) != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x2e, 0x40000000, 0x1000, 0);
            goto tail;
        }
        if (_ZN6Player6IsAnimEj(((char*)this), 0x4e) != 0) {
            mAngleY = (s16)(mAngleY + 0x8000);
        }
        if ((*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0)
            && AngleDiff(mAngleY, mPrevAngleY) >= 0x4000) {
            if (AngleDiff(mDesiredAngleY, mAngleY) >= 0x4000) {
                mAngleY = mPrevAngleY;
            } else {
                mHorzSpeed = 0 - mHorzSpeed;
            }
        }
        mPrevAngleY = mAngleY;
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        if (_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_0211013c) != 0
            && mStateStep != 0
            && mHorzSpeed >= Player_ScaleByCharFactor(((char*)this), 0x1c000)) {
            unk_760 = 0x4000;
        }
        return 1;
    }

    if (func_ov002_020e3078(((char*)this), data_ov002_0211019c) == 0) {
        mHorzSpeed = 0;
    }

tail:
    Player_AdvanceAnims(((char*)this));
    return 1;
}
