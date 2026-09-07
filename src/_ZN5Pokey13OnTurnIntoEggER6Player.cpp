//cpp
// @symbol _ZN5Pokey13OnTurnIntoEggER6Player
/* daSanbo_c::OnTurnIntoEgg -- vtable slot 19, recovered from vtable slot identity.
 * Only the head segment (actorID 0xf0) pays out a coin; every segment marks
 * itself for destruction. */
#include "Pokey.h"
#include "Player.h"

int Pokey::OnTurnIntoEgg(Player &player)
{
    int flag = (actorID == 0xf0);
    if (flag)
        GivePlayerCoins(player, 1, 2);
    MarkForDestruction();
}
