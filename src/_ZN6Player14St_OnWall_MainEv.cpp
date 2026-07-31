//cpp
// @symbol _ZN6Player14St_OnWall_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int s32;
typedef short s16;
typedef long long s64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern void func_ov002_020c0364(char* c, u32 a);
extern void func_ov002_020cabe0(char* c);
extern int func_ov002_020c5244();
extern int func_ov002_020d36d8(char* c, int a);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int _ZN4cstd5atan2E5Fix12IiES1_(Fix12 a, int b);
extern void _Z14ApproachLinearRsss(s16* v, s16 t, s16 s);
extern int func_ov002_020bf224(char* c, int a, int b);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int func_ov002_020d4d88(char* c, int a, int b);
extern void Player_AdvanceAnims(char* c);

extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern int data_ov002_0211013c[];
}

int Player::St_OnWall_Main()
{
    u16 f = mStateFlags;
    u16 b0 = f & 1;
    if (b0 != 0) {
        func_ov002_020c0364(((char*)this), 3);
        return 1;
    }
    u16 b1 = f & 4;
    if (b1 != 0) {
        func_ov002_020cabe0(((char*)this));
        return 1;
    }
    if (func_ov002_020c5244() != 0)
        return 1;
    if (func_ov002_020d36d8(((char*)this), 0) != 0) {
        mTargetAngleY = mAngleY;
        return 1;
    }
    if ((mClsnFlags & 2) == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        return 1;
    }
    if (mStateStep == 2) {
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) == 0) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            return 1;
        }
        int a = _ZN4cstd5atan2E5Fix12IiES1_(unk_560, unk_568) + 0x8000;
        int d = AngleDiff((s16)a, mDesiredAngleY);
        if (d >= 0x4000) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            return 1;
        }
        if (d >= 0x1555) {
            mStateStep = 0;
            if ((s16)a == (s16)(mDesiredAngleY + d))
                mStateStep = 1;
            _ZN6Player14St_OnWall_InitEv(((char*)this));
            return 1;
        }
        _Z14ApproachLinearRsss((s16*)((char*)&mAngleY), (s16)a, 0x800);
        mTargetAngleY = a;
        mHorzSpeed = func_ov002_020bf224(((char*)this), 0xa000, 0x2000);
        func_ov002_020eee3c(((char*)this) + 0x380, ((char*)this));
    } else {
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            int ang = _ZN4cstd5atan2E5Fix12IiES1_(unk_560, unk_568);
            int d = AngleDiff((s16)(ang + 0x8000), mDesiredAngleY);
            if (d >= 0x4000) {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
                return 1;
            }
            if (d < 0x1555) {
                mStateStep = 2;
                _ZN6Player14St_OnWall_InitEv(((char*)this));
                return 1;
            }
            int a2 = ang + 0x8000;
            _Z14ApproachLinearRsss((s16*)((char*)&mAngleY), (s16)a2, 0x800);
            u32 anim;
            if ((s16)a2 == (s16)(mDesiredAngleY + d)) {
                mTargetAngleY = ang + 0x4000;
                anim = 0x5d;
            } else {
                mTargetAngleY = ang - 0x4000;
                anim = 0x5c;
            }
            Fix12 spd = (Fix12)(((s64)mHorzSpeed * 0x600 + 0x800) >> 12);
            if (spd < 0x400)
                spd = 0x400;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), anim, 0, spd, 0);
            func_ov002_020d4d88(((char*)this), func_ov002_020bf224(((char*)this), 0x6000, 0x1000), 0x1000);
        } else {
            mHorzSpeed = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x5e, 0, 0x1000, 0);
        }
    }
    Player_AdvanceAnims(((char*)this));
    return 1;
}
