//cpp
// @symbol _ZN9PowerStar13OnTurnIntoEggER6Player
// recovered name: PowerStar_OnTurnIntoEgg
/* daStar_c::OnTurnIntoEgg -- vtable slot 19, verified against ov002 relocs.txt:
 * _ZTV9PowerStar (0x0210ab3c) + 0x4c -> 0x020e8edc, exactly this placeholder's
 * former address (former name func_ov002_020e8edc). The ROM body is a
 * tail-call veneer (`ldr ip, [pc]; bx ip`) to func_ov002_020e8e80, passing
 * `this`/`player` straight through unchanged.
 * Matched byte-for-byte with mwccarm 2004/b56 (ov002).
 */
#include "PowerStar.h"
#include "Player.h"

extern "C" int func_ov002_020e8e80(PowerStar *thiz, Player &player);

int PowerStar::OnTurnIntoEgg(Player &player)
{
    return func_ov002_020e8e80(this, player);
}
