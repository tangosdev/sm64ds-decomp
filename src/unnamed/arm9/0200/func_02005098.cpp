//cpp
#include "dCamera_c.h"
#include "daPly_c.h"

/* Camera state main: frame the target player's position and ground height
 * in the current mode (func_0200bec4, with its mFlags 0x20000 argument), then
 * pull the eye to distance 0x80000 (ApproachDist). */
int func_02005098(dCamera_c *cam) {
    daPly_c *player = (daPly_c *)cam->mTargetPlayer;
    cam->func_0200bec4(&player->mPos.x, player->mGroundY, (char *)cam->mMode, 0x20000);
    cam->ApproachDist(0x80000);
    return 1;
}
