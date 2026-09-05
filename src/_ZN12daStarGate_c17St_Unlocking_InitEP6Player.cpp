//cpp
// @symbol _ZN12daStarGate_c17St_Unlocking_InitEP6Player
#include "daStarGate_c.h"
#include "Player.h"

bool daStarGate_c::St_Unlocking_Init(Player *player)
{
    player->OpenBigDoor();
    return true;
}
