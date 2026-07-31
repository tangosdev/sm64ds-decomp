//cpp
#include "types.h"
// @symbol _ZN6Player13OnYoshiTryEatEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
/* Player::OnYoshiTryEat() at 0x020e69b8 (ov002) -- vtable slot 17.
 * Overrides Actor's slot 17; returns an OnYoshiEatReturnVal of 1
 * (Yoshi cannot eat the Player). The implicit Player* this is unused.
 */
struct Player;

u32 Player::OnYoshiTryEat()
{
    (void)this;
    return 1;
}
