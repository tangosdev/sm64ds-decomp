//cpp
#include "types.h"
// @symbol _ZN6Player15St_Respawn_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *c, u32 a, int b, int f, u32 g);
extern int _ZN6Player12FinishedAnimEv(void *c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *c, void *s);
extern void Player_AdvanceAnims(void *c);
extern u8 data_0209f250;
extern int data_ov002_02110154[];
}

int Player::St_Respawn_Main()
{
    switch (mStateStep) {
    case 0: {
        u16 t = mStateTimer;
        if (t != 0) {
            if (t == 1) {
                mVertAccel = -0x4000;
                if (mPlayerNo == data_0209f250) {
                    FUN_02029980();
                    mStateWaitTimer = 0x26;
                }
            }
        } else {
            if (mIsAirborne == 0) {
                u32 zero = 0;
                _ZN6Player7SetAnimEji5Fix12IiEj(((char *)this), 0x55, 0x40000000, 0x1000, zero);
                mStateStep += 1;
            }
        }
        break;
    }
    case 1:
        if (_ZN6Player12FinishedAnimEv(((char *)this)) != 0) {
            int z = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(((char *)this), 0x47, z, 0x1000, z);
            mStateStep += 1;
        }
        break;
    case 2:
        if (mStateWaitTimer == 0) {
            if (mPlayerNo == data_0209f250) {
                FUN_02029934();
            }
            _ZN6Player11ChangeStateERNS_5StateE(((char *)this), data_ov002_02110154);
        }
        break;
    }
    Player_AdvanceAnims(((char *)this));
    return 1;
}
