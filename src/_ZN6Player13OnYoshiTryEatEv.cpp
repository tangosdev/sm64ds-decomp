//cpp
#include "types.h"
// @symbol _ZN6Player13OnYoshiTryEatEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
/* Player::OnYoshiTryEat() at 0x020e69b8 (ov002) -- vtable slot 18, not 17.
 * Slot 17 is _ZN6PlayerD0Ev; slot 18 is the first of dActor_c's own virtuals.
 * See notes/actor-vtables.md, which was read out of _ZTV6Player.
 *
 * Returns an OnYoshiEatReturnVal of 1 (Yoshi cannot eat the Player). The
 * implicit Player* this is unused.
 *
 * Returns int rather than u32 to match dActor_c::OnYoshiTryEat -- CW rejects an
 * override whose return type differs from the base declaration.
 */
int Player::OnYoshiTryEat()
{
    (void)this;
    return 1;
}
