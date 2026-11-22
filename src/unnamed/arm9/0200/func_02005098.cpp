//cpp
#include "dCamera_c.h"
#include "daPly_c.h"

/* Camera state main: frame the target player's position and ground height
 * in the current mode (func_0200bec4, with its mFlags 0x20000 argument), then
 * pull the eye to distance 0x80000 (ApproachDist). */
int dCamera_c::func_02005098() {
    daPly_c *player = (daPly_c *)this->mTargetPlayer;
    this->func_0200bec4(&player->mPos, player->mGroundY, (char *)this->mMode, 0x20000);
    this->ApproachDist(0x80000);
    return 1;
}
