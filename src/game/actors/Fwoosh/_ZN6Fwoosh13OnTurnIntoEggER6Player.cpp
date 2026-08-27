//cpp
// @symbol _ZN6Fwoosh13OnTurnIntoEggER6Player
// recovered name: Fwoosh_OnTurnIntoEgg
/* daHyuhyu_c::OnTurnIntoEgg -- vtable slot 19, verified against ov091
 * relocs.txt: _ZTV6Fwoosh (0x021353ac) + 0x4c -> 0x0213448c, exactly this
 * placeholder's former address (former name func_ov091_0213448c). The ROM
 * body is a tail-call veneer (`ldr ip, [pc]; bx ip`) straight to
 * fBase_c::MarkForDestruction, discarding the player argument.
 * Matched byte-for-byte with mwccarm 2004/b56 (ov091).
 */
#include "Fwoosh.h"
#include "Player.h"

int Fwoosh::OnTurnIntoEgg(Player &player)
{
    MarkForDestruction();
}
