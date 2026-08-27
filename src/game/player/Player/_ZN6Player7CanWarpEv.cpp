//cpp
// @symbol _ZN6Player7CanWarpEv
/* recovered: named members + shared header, real C++ method
 *
 * Warping is allowed from exactly two states. One of them is now spelt as the
 * static member it is -- Player::ST_WAIT, which the compiler mangles to
 * _ZN6Player7ST_WAITE for us -- instead of a hand-written extern. The other is
 * still reached by its ov002 address; it has no recovered name yet.
 *
 * The file's own `struct State;` stand-in is gone with it: IsState really
 * takes a Player::State&, which is what its mangling says, and the local
 * forward declaration only ever existed to give the extern something to point
 * at.
 */
#include "Player.h"

extern "C" {
extern Player::State data_ov002_021102a4;
extern int _ZN6Player7IsStateERNS_5StateE(void *c, Player::State *st);
}

int Player::CanWarp()
{
    if (_ZN6Player7IsStateERNS_5StateE((void *)this, &ST_WAIT) ||
        _ZN6Player7IsStateERNS_5StateE((void *)this, &data_ov002_021102a4))
        return 1;
    return 0;
}
