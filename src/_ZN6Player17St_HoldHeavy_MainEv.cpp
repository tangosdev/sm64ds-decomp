//cpp
#include "types.h"
// @symbol _ZN6Player17St_HoldHeavy_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int _ZN6Player6IsAnimEj(void* c, u32 anim);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern void func_ov002_020da9d4(void* c);
extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern int func_ov002_020bf224(void* c, int a, int b);
extern void func_ov002_020d4d88(void* c, int a, int b);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern void Player_AdvanceAnims(void* c);

extern int data_ov002_0211013c[];
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern int data_ov002_021105d4[];
extern int data_ov002_021101b4[];
extern s16 data_0209f4a0[];
extern u8 data_0209f4ac[];
extern u16 data_0209f49c[];
}

int Player::St_HoldHeavy_Main()
{
    int var_r1 = 0;

    if (mHeldObj == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
        return 1;
    }

    if (mStateStep == 0) {
        u32 anim = data_ov002_020ff254[mStateWork];
        if (_ZN6Player6IsAnimEj(((char*)this), anim)) {
            if (_ZN6Player12FinishedAnimEv(((char*)this))) {
                mStateStep = 1;
            } else {
                u32 arg = (u8)mParam;
                int modelIdx = _ZNK6Player14GetBodyModelIDEjb(((char*)this), arg, 0);
                char* animPtr = *(char**)(((char*)this) + modelIdx * 4 + 0xdc) + 0x50;
                if (_ZNK9Animation12WillHitFrameEi(animPtr, 6)) {
                    int* heavy = *(int**)((char*)&mHeldObj);
                    if (heavy != 0) {
                        *(int*)(((long long)(int)((char*)heavy + 0xb0))) |= 0x4000;
                    }
                }
            }
        }
        _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed), 0, 0x1000);
        goto end;
    }

    {
        int off = data_020a0e40 * 0x18;
        u16 flags = *(u16*)((char*)data_0209f49e + off);
        if (flags & 1) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021105d4);
            return 1;
        }

        if (mIsAirborne != 0) {
            func_ov002_020da9d4(((char*)this));
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101b4);
            unk_6b4 = 0xa;
            return 1;
        }

        {
            s16 t = *(s16*)((char*)data_0209f4a0 + off);
            if (t != 0) {
                ApproachAngle((short*)((char*)&mTargetAngleY), mDesiredAngleY, 8, 0x1000, 0x400);
                var_r1 = data_ov002_020ff1d0[mParam];
            }
        }
    }

    {
        int off2 = data_020a0e40 * 0x18;
        if (*((u8*)data_0209f4ac + off2) == 0) {
            if (!(*(u16*)((char*)data_0209f49c + off2) & 0x800)) {
                var_r1 >>= 1;
            }
        } else {
            var_r1 = func_ov002_020bf224(((char*)this), var_r1, 0);
        }
        func_ov002_020d4d88(((char*)this), var_r1, 0x1000);
    }

    if (mHorzSpeed == 0) {
        mAngleY = mTargetAngleY;
    } else if (mParam == 1) {
        ApproachAngle((short*)((char*)&mAngleY), mTargetAngleY, 0x10, 0x1000, 0x100);
    } else {
        ApproachAngle((short*)((char*)&mAngleY), mTargetAngleY, 8, 0x2000, 0x400);
    }

    if (mHorzSpeed == 0) {
        if (_ZN6Player12FinishedAnimEv(((char*)this)) ||
            !_ZN9Animation8GetFlagsEv(
                *(char**)(((char*)this) + _ZNK6Player14GetBodyModelIDEjb(((char*)this), (u8)mParam, 0) * 4 + 0xdc) + 0x50)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), data_ov002_020ff254[mStateWork + 2], 0, 0x1000, 0);
        }
    } else {
        func_ov002_020d1f78(((char*)this), data_ov002_020ff254[mStateWork + 4]);
    }

end:
    Player_AdvanceAnims(((char*)this));
    return 1;
}
