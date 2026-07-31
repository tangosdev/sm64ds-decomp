//cpp
#include "types.h"
// @symbol _ZN6Player14St_Crouch_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020bf90c(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int _ZN6Player6IsAnimEj(void* c, u32 a);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern void func_ov002_020c06fc(void* c, u32 a);
extern int func_ov002_020dd2f4(void* c);
extern void func_ov002_020c0364(void* c, u32 a);
extern void Player_AdvanceAnims(void* c);

extern u16 data_0209f49c[];
extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern int data_ov002_021101b4[];
extern int data_ov002_02110514[];
extern int data_ov002_0211013c[];
}

int Player::St_Crouch_Main()
{
    func_ov002_020bf90c(((char*)this));

    if (mIsAirborne != 0) {
        int d = mPosY - mGroundY;
        if (d >= 0x32000) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_021101b4);
            return 1;
        }
    }

    switch (mStateStep) {
    case 0:
        if (_ZN6Player6IsAnimEj(((char*)this), 0x2d) && _ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x2c, 0, 0x1000, 0);
            mStateStep = 1;
        }
        if (func_ov002_020d12b0(((char*)this)))
            return 1;
        func_ov002_020d1204(((char*)this));
        break;
    case 1:
        if ((*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 0x400) != 0
            || mHorzSpeed != 0
            || func_ov002_020d1164(((char*)this)) != 0) {
            if (mHorzSpeed != 0) {
                func_ov002_020c06fc(((char*)this), 0x4000);
                func_ov002_020dd2f4(((char*)this));
            } else {
                if (mIsMega == 0) {
                    if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) >= 0x200) {
                        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110514);
                        return 1;
                    }
                }
            }
            u16 bit = mStateFlags & 1;
            if (bit != 0) {
                func_ov002_020c0364(((char*)this), 1);
                return 1;
            }
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x2e, 0x40000000, 0x1000, 0);
            mStateStep = 2;
        }
        if (func_ov002_020d12b0(((char*)this)))
            return 1;
        if (func_ov002_020d1204(((char*)this)) != 0)
            return 1;
        break;
    case 2:
        if (_ZN6Player6IsAnimEj(((char*)this), 0x2e) && _ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_0211013c);
            unk_6b0 = 0xa;
        } else {
            func_ov002_020d1204(((char*)this));
        }
        break;
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
