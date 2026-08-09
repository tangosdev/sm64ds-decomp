//cpp
// @symbol _ZNK6Player14GetBodyModelIDEjb
/* recovered: named members + shared header, real C++ method
 *
 * Metal cap overrides whichever body model the caller asked for. Which metal
 * flag is consulted depends on the second argument -- mIsMetal for the player
 * itself, unk_6fa for the other one.
 *
 * const, and the _ZNK in the symbol is what says so: declared without it the
 * compiler emits _ZN..., which names a symbol that does not exist.
 */
#include "Player.h"

unsigned int Player::GetBodyModelID(unsigned int a, bool b_) const
{
    if (b_ == 0) {
        if (mIsMetal) a = 4;
    } else {
        if (unk_6fa) a = 4;
    }
    return a;
}
