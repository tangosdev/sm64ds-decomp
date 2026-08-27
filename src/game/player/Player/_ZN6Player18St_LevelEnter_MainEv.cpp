//cpp
// @symbol _ZN6Player18St_LevelEnter_MainEv
/* recovered: named members + shared header, real C++ method
 *
 * Level-entry cutscene, per frame. mStateWork indexes a table of
 * POINTERS TO MEMBER FUNCTION -- the same ARM/Itanium pmf representation
 * Player::State uses -- and the selected step runs against the player. Six of
 * the mStateStep values additionally publish the player to the global at
 * data_0209f318+0x118, presumably as the camera's focus.
 */
#include "Player.h"

extern void (Player::*data_ov002_0211075c[])();

struct Obj { char pad[0x118]; void *f118; };
extern Obj *data_0209f318;

extern "C" {
extern void Player_AdvanceAnims(void *c);
}

int Player::St_LevelEnter_Main()
{
    switch (mStateStep) {
    case 8: case 9: case 11: case 12: case 16: case 18:
        data_0209f318->f118 = this;
        break;
    case 10: case 13: case 14: case 15: case 17:
    default:
        break;
    }
    (this->*data_ov002_0211075c[mStateWork])();
    if (mStateArg == 0) Player_AdvanceAnims(this);
    return 1;
}
