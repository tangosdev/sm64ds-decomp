//cpp
#include "types.h"
// @symbol _ZN6Player16St_SideFlip_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020eeca8(void* a, void* b);
extern int _ZN6Player7IsStateERNS_5StateE(void* c, void* s);
extern void func_ov002_020e28d4(void* c, u32 a, u32 b);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void* v);
extern void Player_AdvanceAnims(void* c);

extern int data_ov002_02110424[];
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern int data_ov002_0211052c[];
extern u16 data_0209f49c[];
extern int data_ov002_02110454[];
}

int Player::St_SideFlip_Main()
{
    func_ov002_020eeca8(((char*)this) + 0x380, ((char*)this));

    if (_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_02110694)) {
        if (mVertSpeed < 0) {
            mHorzSpeed = 0x12000;
        }
        func_ov002_020e28d4(((char*)this), 0, 0x800);
    } else {
        func_ov002_020e28d4(((char*)this), 0x1800, 0x800);
    }

    if (mIsAirborne == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110424);
    } else {
        if (!_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_021101fc)) {
            u16 r0v = *(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18);
            if (r0v & 0x400) {
                if (_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_0211052c)) {
                    mTargetAngleY = mAngleY;
                }
            }
            if (func_ov002_020e2664(((char*)this))) {
                return 1;
            }
        } else {
            if (_ZN6Player12FinishedAnimEv(((char*)this))) {
                _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x54, 0x40000000, 0x1000, 0);
            }
        }

        mVertAccel = -0x4000;

        if (mVertSpeed >= 0) {
            u16 r1v = *(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18);
            if ((r1v & 2) != 0
                || _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_021101e4)
                || _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_021101fc)
                || _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_0211052c)) {
                mVertAccel = -0x3400;
            }
        } else {
            if (mParam == 1
                && _ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_0211052c)) {
                unk_6e6 = 0;
                _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110454);
                return 1;
            }
        }

        if (_ZN6Player7IsStateERNS_5StateE(((char*)this), data_ov002_021101e4)) {
            int id = _ZNK6Player14GetBodyModelIDEjb(((char*)this), mParam & 0xff, 0);
            void* anim = *(void**)(((char*)this) + (id << 2) + 0xdc);
            u32 w = *(u32*)((char*)(((long long)(int)((char*)anim + 0x50))) + 8);
            if ((u16)(w >> 12) == 0x10) {
                _ZN5Sound9PlayBank0EjRK7Vector3(0xf, ((char*)this) + 0x74);
            }
        }
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
