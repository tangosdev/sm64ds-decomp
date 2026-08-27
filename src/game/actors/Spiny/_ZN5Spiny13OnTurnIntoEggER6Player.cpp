//cpp
// @symbol _ZN5Spiny13OnTurnIntoEggER6Player
/* daTgz_c::OnTurnIntoEgg -- vtable slot 19, recovered from vtable slot identity.
 * Gives the player 1 coin (as a cap-collection coin if Yoshi is wearing the cap,
 * otherwise as an egg coin), then marks this actor for destruction. */
#include "Spiny.h"
#include "Player.h"

int Spiny::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap())
        GivePlayerCoins(player, 1, 0);
    else
        player.RegisterEggCoinCount(1, 0, 0);
    MarkForDestruction();
}
