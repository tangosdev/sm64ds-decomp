//cpp
// @symbol _ZN6Player18St_DizzyStars_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"


int Player::St_DizzyStars_Init()
{
    mStateTimer = 0x12c;
    *(int *)(((int)((char *)this) + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x80;
    return 1;
}
