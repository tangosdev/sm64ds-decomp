//cpp
// @symbol _ZN6Player17St_HoldLight_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;
typedef s32 Fix12;

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020c0434(void* c);
extern void func_ov002_020c0364(void* c, u32 arg);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern int _ZN6Player6IsAnimEj(void* c, u32 anim);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int func_ov002_020e0ccc(void* c, short* st);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern int func_ov002_020c5244(void* c);
extern int Player_ReleaseHeldActor(void* c);
extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern int func_ov002_020bf224(void* c, int a, int b);
extern void func_ov002_020d4d88(void* c, int a, int b);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern void func_ov002_020bedd4(void* c);

extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern u8 data_0209f4ac[];
extern u16 data_0209f49c[];
extern u16 data_0209f49e[];
extern int data_ov002_0211013c[];
extern int data_ov002_021105d4[];
extern int data_ov002_021101b4[];
}

int Player::St_HoldLight_Main()
{
    if (mHeldObj == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        return 1;
    }
    if (func_ov002_020c0434(((char*)this))) {
        func_ov002_020c0364(((char*)this), 3);
        return 1;
    }

    if (mStateStep == 0) {
        _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), 0, 0x800);
        if (_ZN6Player6IsAnimEj(((char*)this), 0x2f) || _ZN6Player6IsAnimEj(((char*)this), 0x86)) {
            if (_ZN6Player12FinishedAnimEv(((char*)this))) {
                if (func_ov002_020e0ccc(((char*)this), *(short**)((char*)&mHeldObj))) {
                    return 1;
                }
                mTargetAngleY = mAngleY;
                mHorzSpeed = 0;
                mStateStep = 1;
            } else {
                u32 arg = (u8)mParam;
                int modelIdx = _ZNK6Player14GetBodyModelIDEjb(((char*)this), arg, 0);
                char* anim = *(char**)(((char*)this) + modelIdx * 4 + 0xdc) + 0x50;
                if (_ZNK9Animation12WillHitFrameEi(anim, 6)) {
                    int* light = *(int**)((char*)&mHeldObj);
                    if (light != 0) {
                        int* p = (int*)(((long long)(int)((char*)light + 0xb0)));
                        *p |= 0x4000;
                    }
                }
            }
        }
        goto end;
    }

    if (func_ov002_020c5244(((char*)this))) {
        return 1;
    }

    if (mSinkDepth > 0x19000) {
        Player_ReleaseHeldActor(((char*)this));
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        return 1;
    }

    int var_r4 = 0;
    if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        s16 t = mDesiredAngleY;
        if (mStateArg != 0) {
            mTargetAngleY = t;
            mStateArg = 0;
        }
        ApproachAngle((short*)((char*)&mTargetAngleY), t, 4, 0x2000, 0x800);
        var_r4 = data_ov002_020ff1c0[mParam];
    }

    if (*((u8*)data_0209f4ac + data_020a0e40 * 0x18) == 0) {
        if (!(*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 0x800)) {
            var_r4 = (s32)(((s64)var_r4 * 0xa00 + 0x800) >> 0xc);
        }
    } else {
        var_r4 = func_ov002_020bf224(((char*)this), var_r4, 0);
    }

    mStateArg = 0;
    if (mHorzSpeed == 0) {
        mStateArg = 1;
        mAngleY = mTargetAngleY;
    } else {
        ApproachAngle((short*)((char*)&mAngleY), mTargetAngleY, 8, 0x2000, 0x800);
    }

    func_ov002_020d4d88(((char*)this), var_r4, 0x1000);

    {
        u16 flags = *(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18);
        if (flags & 1) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021105d4);
            return 1;
        }
        if (flags & 2) {
            mJumpComboTimer = 0;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211019c);
            return 1;
        }
        if (flags & 0x400) {
            if (Player_ReleaseHeldActor(((char*)this))) {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            }
            return 1;
        }
    }

    if (mIsAirborne != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101b4);
        return 1;
    }
    if (func_ov002_020d9dcc(((char*)this))) {
        return 1;
    }

    if (mHorzSpeed == 0) {
        if (_ZN6Player12FinishedAnimEv(((char*)this)) ||
            !_ZN9Animation8GetFlagsEv(
                *(char**)(((char*)this) + _ZNK6Player14GetBodyModelIDEjb(((char*)this), (u8)mParam, 0) * 4 + 0xdc) + 0x50)) {
            int* light = *(int**)((char*)&mHeldObj);
            u32 animId = 0x33;
            if (light != 0) {
                int b = (*(int*)((char*)light + 0xb0) & 0x8000) != 0;
                if (b) {
                    animId = 0x87;
                    if (mParam == 2) {
                        int b2 = (*(u16*)((char*)light + 0xc) == 0xce);
                        if (b2) {
                            animId = 0x33;
                        }
                    }
                }
            }
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), animId, 0, 0x1000, 0);
        }
    } else {
        func_ov002_020d1f78(((char*)this), 0x48);
    }

end:
    func_ov002_020bedd4(((char*)this));
    return 1;
}
