//cpp
#include "types.h"
// @symbol _ZN6Player17St_LedgeHang_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12i b, u32 d);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void Player_AdvanceAnims(void* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern s16 data_0209f4a0[];
}

int Player::St_LedgeHang_Main()
{
    int v = mGroundY;
    if (v != (int)0x80000000) {
        int d = v - mPosY;
        if (d < 0) d = -d;
        if (d < 0x28000) {
            mPosY = v;
            mIsAirborne = 0;
            if (mFloorNormalY < 0xddb) {
                func_ov002_020d0948(((char*)this));
                return 1;
            }
        }
    }

    if (mIsAirborne != 0) {
        func_ov002_020d0948(((char*)this));
        return 1;
    }

    if (mStateStep == 0) {
        if (_ZN6Player12FinishedAnimEv(((char*)this))) {
            _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x21, 0, 0x1000, 0);
            *(u8*)(((int)((char*)this) + 0x6e3)) =
                *(u8*)(((int)((char*)this) + 0x6e3)) + 1;
        }
    } else {
        if ((!func_ov002_020cfaf0(((char*)this))
             && (*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2))
            || func_ov002_020cfea4(((char*)this))) {
            mStateStep = 1;
            _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110004);
        } else {
            if (func_ov002_020cfbdc(((char*)this)))
                return 1;
            if (*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 0x400) {
                func_ov002_020d0948(((char*)this));
                return 1;
            }
            {
                if (mStateTimer == 0
                    && *(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
                    if (AngleDiff(mDesiredAngleY, mAngleY) >= 0x4000) {
                        func_ov002_020d0948(((char*)this));
                    } else if (!func_ov002_020cfaf0(((char*)this))) {
                        mStateStep = 0;
                        _ZN6Player11ChangeStateERNS_5StateE(((char*)this), data_ov002_02110004);
                    }
                }
            }
        }
    }

    Player_AdvanceAnims(((char*)this));
    return 1;
}
