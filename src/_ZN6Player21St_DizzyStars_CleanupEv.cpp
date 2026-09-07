//cpp
#include "Player.h"
int Player::St_DizzyStars_Cleanup()
{
    char *self = (char *)this;
    mFlags &= ~0x80u;
    return 1;
}