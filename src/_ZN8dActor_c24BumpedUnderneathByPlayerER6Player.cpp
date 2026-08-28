//cpp
#include "Player.h"

int dActor_c::BumpedUnderneathByPlayer(Player &player)
{
    if (player.mIsAirborne != 0 && player.mVertSpeed > 0
        && player.mPosY < mPosY)
        return 1;

    return 0;
}
