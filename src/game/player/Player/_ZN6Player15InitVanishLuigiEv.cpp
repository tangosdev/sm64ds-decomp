//cpp
// @symbol _ZN6Player15InitVanishLuigiEv
/* recovered: named members + shared header, real C++ method
 *
 * Starts the vanish cap: sets the flag, arms mPowerupTimer to 0x258 (the same 600
 * frames mBalloonTimer uses), then the sound and effect calls.
 *
 * Note the original wrote mPowerupTimer as `c + 0x600 + 0xae` rather than 0x6ae --
 * the split is how the compiler addressed it, not a second field.
 */
#include "Player.h"
extern "C" {
extern int func_ov002_020bda48(void*);
extern int func_ov002_020bd9ec(void*, int);
extern int func_ov002_020c43c4(void*, int);
}

void Player::InitVanishLuigi()
{
    func_ov002_020bda48(this);
    mIsVanish = 1;
    mPowerupTimer = 0x258;
    func_ov002_020bd9ec(this, 0x33);
    func_ov002_020c43c4(this, 3);
}
