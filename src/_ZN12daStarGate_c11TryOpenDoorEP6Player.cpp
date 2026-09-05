//cpp
// @symbol _ZN12daStarGate_c11TryOpenDoorEP6Player
#include "daStarGate_c.h"
#include "Player.h"

extern "C" void SetTouchScreenDelay(void);

void daStarGate_c::TryOpenDoor(Player *player)
{
    int a;
    int z = mScaleZ;
    if (z < 0) a = mAngleY;
    else a = mAngleY + 0x8000;
    if (player->TryEnterStarDoor(*(Vector3 *)&mPosX, a))
        ChangeState(&ST_OPEN_CLOSE, player);
    SetTouchScreenDelay();
}
