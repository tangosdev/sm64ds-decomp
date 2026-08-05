//cpp
// @symbol _ZN6Player16St_DebugFly_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void Player_DisableInteraction(void *);
}

int Player::St_DebugFly_Init()
{
    Player_DisableInteraction(((char *)this));
    mHorzSpeed = 0;
    mVertSpeed = 0;
    mVertAccel = 0;
    return 1;
}
