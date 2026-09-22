//cpp
// @symbol _ZN6Player12St_Spin_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Spin-jump, per frame. Landing hands off to another state after snapping the
 * facing angle back to mPrevAngleY. The global bit at data_0209f49c selects
 * between two spin rates and two terminal velocities.
 */
#include "Player.h"
extern u8 data_020a0e40;
extern u16 data_0209f49c[];
extern char data_ov002_02110424;
extern "C" {
extern void func_ov002_020e28d4(void *thiz, int a, int b);
extern void Player_AdvanceAnims(void *thiz);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *thiz, void *st);
}

int Player::St_Spin_Main()
{
    func_ov002_020e28d4(this, 0x1000, 0x1000);
    if (mIsAirborne == 0) {
        mAngleY = mPrevAngleY;
        _ZN6Player11ChangeStateERNS_5StateE(this, &data_ov002_02110424);
    }
    if (*(u16*)((char*)data_0209f49c + (&data_020a0e40)[0] * 0x18) & 2) {
        mTerminalVelocity = -0x9000;
        mAngleY += 0x2000;
    } else {
        mTerminalVelocity = -0xc000;
        mAngleY += 0x1800;
    }
    Player_AdvanceAnims(this);
    return 1;
}
