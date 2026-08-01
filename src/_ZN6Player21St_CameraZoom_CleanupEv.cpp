//cpp
// @symbol _ZN6Player21St_CameraZoom_CleanupEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"


int Player::St_CameraZoom_Cleanup()
{
    *(unsigned short *)(((long long)(int)((char *)&mStateFlags))) &= ~4;
    unk_715 = 0;
    return 1;
}
