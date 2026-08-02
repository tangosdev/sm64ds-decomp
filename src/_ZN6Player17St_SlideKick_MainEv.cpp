//cpp
#include "types.h"
// @symbol _ZN6Player17St_SlideKick_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020bf90c(void* c);
extern void func_ov002_020c06fc(void* c, u32 flag);
extern int func_ov002_020dd2f4(void* c);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int _ZN6Player6IsAnimEj(void* c, u32 id);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020c0688(void* c);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void _ZN12CylinderClsn6UpdateEv(void* c);
extern void Player_AdvanceAnims(void* c);

extern int data_ov002_021104e4[];
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern int data_ov002_021101b4[];
}

int Player::St_SlideKick_Main()
{
    func_ov002_020bf90c(((char*)this));
    if (mIsAirborne == 0) {
        mIsSlidingOnGround = 1;
        func_ov002_020c06fc(((char*)this), 0x4000);
        func_ov002_020dd2f4(((char*)this));
    }

    if (mHorzSpeed != 0)
        goto La8;

    if (_ZN6Player12FinishedAnimEv(((char*)this)) == 0)
        goto L1d8;

    if (_ZN6Player6IsAnimEj(((char*)this), 0x67) == 0)
        goto L88;

    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x66, 0x40000000, 0x1000, 0);
    goto L1d8;

L88:
    mPrevAngleY = mAngleY;
    mStateStep = 1;
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021104e4);
    goto L1d8;

La8:
    if (func_ov002_020c0688(((char*)this)) == 0)
        goto L14c;

    if (mStateWork == 0) {
        if (mPrevVertSpeed < 0) {
            u16 bit = *(u16*)(((char*)this) + 0x600 + 0xce) & 1;
            if (bit == 0) {
                int t = -mPrevVertSpeed;
                if (t > 0x14000) t = 0x14000;
                mVertSpeed = t;
                mStateWork = 1;
            }
        }
    }

    {
        u16 r1 = *(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18);
        if ((r1 & 1) || (r1 & 2)) {
            if (mSlideType == 0) {
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101fc);
            }
        }
    }
    mStateStep = 0;
    goto L18c;

L14c:
    *(u8*)(((int)((char*)this) + 0x6e3)) =
        *(u8*)(((int)((char*)this) + 0x6e3)) + 1;
    if (mStateStep <= 0x1e)
        goto L18c;
    if (mPosY - mGroundY <= 0x64000)
        goto L18c;
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101b4);

L18c:
    {
        int id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam & 0xff, 0);
        void* anim = *(void**)(((char*)this) + (id << 2) + 0xdc);
        u32 w = *(u32*)((char*)(((long long)(int)((char*)anim + 0x50))) + 8);
        if ((u16)(w >> 12) < 4)
            goto L1d8;
        func_ov002_020dbaec(((char*)this));
        _ZN12CylinderClsn5ClearEv((char*)&mAttackClsn);
        _ZN12CylinderClsn6UpdateEv((char*)&mAttackClsn);
    }

L1d8:
    Player_AdvanceAnims(((char*)this));
    mPrevVertSpeed = mVertSpeed;
    return 1;
}
