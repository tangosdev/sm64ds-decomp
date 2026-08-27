//cpp
// @symbol _ZN6Player18St_CameraZoom_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * The zoomed-in camera pose. Runs only while data_02092110 is negative. Losing
 * the 0x4 state flag ends it: back to walking, the body model's animation
 * field at +0x50+8 is cleared, and the facing angle is latched. Landing, or
 * any of the 0x8f03 input bits, clears that flag.
 */
#include "Player.h"
extern "C" {
/* ChangeState is still an extern "C" definition, so it is called by its ROM
   name -- but on the real Player::State now, not a local stand-in. */
extern void _ZN6Player11ChangeStateERNS_5StateE(void *, Player::State&);
extern void func_ov002_020cae10(void* c);
extern int func_ov002_020cac60(void* c);
extern void Player_AdvanceAnims(void* self);
}
extern signed char data_02092110;
extern Player::State data_ov002_0211013c;   /* the walk state */
extern unsigned char data_020a0e40;
extern unsigned short data_0209f49e;

int Player::St_CameraZoom_Main()
{
    if (data_02092110 < 0) {
        if ((unsigned short)(mStateFlags & 4) == 0) {
            unsigned int id;
            _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_0211013c);
            id = GetBodyModelID(param1 & 0xff, 0);
            int q = (int)((*(volatile int*)((char*)&mBodyModels + id * 4) + 0x50));
            *(int*)(q + 8) = 0;
            mPrevAngleY = mAngleY;
            return 1;
        }
        if (mIsAirborne) {
            mStateFlags &= ~4;
        }
        func_ov002_020cae10(this);
        if (*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18) & 0x8f03) {
            mStateFlags &= ~4;
        }
        func_ov002_020cac60(this);
    }
    Player_AdvanceAnims(this);
    return 1;
}
