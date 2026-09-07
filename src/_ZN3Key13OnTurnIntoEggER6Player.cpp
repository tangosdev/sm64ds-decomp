//cpp
// @symbol _ZN3Key13OnTurnIntoEggER6Player
// recovered name: Key_OnTurnIntoEgg
/* daObjKey_c::OnTurnIntoEgg -- vtable slot 19, verified against ov089
 * relocs.txt: _ZTV3Key (0x02132ba8) + 0x4c -> 0x02131f04, exactly this
 * placeholder's former address (former name func_ov089_02131f04).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov089).
 */
#include "Key.h"
#include "Player.h"

extern "C" {
extern int func_ov089_02131df4(Key *thiz, Player *player);
extern int func_ov089_02131dcc(Key *thiz, Player *player);
}

int Key::OnTurnIntoEgg(Player &player)
{
    unsigned isMatch = (actorID == 0x11a);
    if (isMatch)
        return func_ov089_02131df4(this, &player);
    return func_ov089_02131dcc(this, &player);
}
