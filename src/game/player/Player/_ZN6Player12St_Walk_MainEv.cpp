//cpp
#include "types.h"
// @symbol _ZN6Player12St_Walk_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020c5244(void);
extern int func_ov002_020d36d8(void* c, int a);
extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern int _ZN6Player6IsAnimEj(void* c, u32 anim);
extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern void func_ov002_020d4540(void* c);
extern void func_ov002_020cabe0(void* c);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern void Player_AdvanceAnims(void* c);

extern u8 data_020a0e40;
extern u8 data_0209f4ae[];
extern s16 data_0209f4a0[];
extern int data_ov002_02110154[];
}

int Player::St_Walk_Main()
{
    if ((u16)(mStateFlags & 0x800)) {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110154);
        return 1;
    }
    if (func_ov002_020c5244()) return 1;
    if (func_ov002_020d36d8(((char*)this), 0)) return 1;

    func_ov002_020d45c0(((char*)this));
    short r5 = mPrevAngleY;
    if (func_ov002_020d3b9c(((char*)this))) return 1;

    func_ov002_020d413c(((char*)this), r5);

    if (mHorzSpeed == 0) {
        mAngleY = mPrevAngleY;
    } else {
        ApproachAngle((short*)((char*)&mAngleY), mPrevAngleY, 4, 0x2000, 0x800);
    }

    if (unk_6e0 != 0) {
        if (!_ZN6Player6IsAnimEj(((char*)this), 0x37)) {
            int result = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2, (int)((char*)&mCamSpacePosX), mHorzSpeed, 0);
            mLoopingSoundHandle = result;
        }
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            if (_ZN6Player6IsAnimEj(((char*)this), 0x36)) {
                if (mHorzSpeed == 0) {
                    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x37, 0x40000000, 0x1000, 0);
                }
            } else {
                if (_ZN6Player6IsAnimEj(((char*)this), 0x37)) {
                    func_ov002_020d4540(((char*)this));
                }
            }
        }
        goto end;
    }

    if (mHorzSpeed != 0) goto label_29c;

    {
        u8 idx = data_020a0e40;
        int off = idx * 0x18;
        u8 b = data_0209f4ae[off];
        int thresh = (b != 2) ? 0x471 : 0x555;
        u8 v710 = unk_710;
        int rhs = thresh - (v710 << 7);
        short lhs = *(s16*)((char*)data_0209f4a0 + off);
        if (lhs < rhs) goto label_1f0;
        if ((u16)(mStateFlags & 0xc) == 0) goto label_29c;
    }

label_1f0:
    if (mIsMega != 0) {
        if (_ZN6Player12FinishedAnimEv(((char*)this)) && _ZN6Player6IsAnimEj(((char*)this), 0x9d)) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110154);
            mStateWork = 1;
            func_ov002_020cabe0(((char*)this));
        }
        goto end;
    }

    if (_ZN6Player12FinishedAnimEv(((char*)this))) goto label_27c;

    {
        u32 arg = (u8)param1;
        int modelIdx = _ZNK6Player14GetBodyModelIDEjb(((char*)this), arg, 0);
        char* anim = *(char**)(((char*)this) + modelIdx * 4 + 0xdc) + 0x50;
        if (_ZN9Animation8GetFlagsEv(anim)) goto end;
    }

label_27c:
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110154);
    mStateWork = 1;
    func_ov002_020cabe0(((char*)this));
    goto end;

label_29c:
    func_ov002_020d4748(((char*)this));
    mStateWork = 1;

end:
    Player_AdvanceAnims(((char*)this));
    return 1;
}
