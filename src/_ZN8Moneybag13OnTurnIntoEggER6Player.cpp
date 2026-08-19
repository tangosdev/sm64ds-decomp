//cpp
// @symbol _ZN8Moneybag13OnTurnIntoEggER6Player
/* daGmch_c::OnTurnIntoEgg -- vtable slot 19, recovered from vtable slot identity.
 * Gives the player 5 coins (as cap-collection coins if Yoshi is wearing the cap,
 * otherwise as egg coins), then kills this actor and tracks it in the death table. */
#include "Moneybag.h"
#include "Player.h"

int Moneybag::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap())
        GivePlayerCoins(player, 5, 0);
    else
        player.RegisterEggCoinCount(5, 0, 0);
    KillAndTrackInDeathTable();
}
