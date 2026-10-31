//cpp
#include "dCamera_c.h"
#include "daPly_c.h"

extern "C" {

/* Parameter types follow the definitions in src/engine/view/d_camera.cpp. */
int func_0200bec4(char *cam, int *pos, int groundY, char *mode, int flags);
int func_02009138(dCamera_c *cam, int dist);

/* Camera state main: frame the target player's position and ground height
 * in the current mode (func_0200bec4, with its mFlags 0x20000 argument), then
 * pull the eye to distance 0x80000 (func_02009138). */
int func_02005098(dCamera_c *cam) {
    daPly_c *player = (daPly_c *)cam->mTargetPlayer;
    func_0200bec4((char *)cam, (int *)&player->mPos, player->mGroundY, (char *)cam->mMode, 0x20000);
    func_02009138(cam, 0x80000);
    return 1;
}
}
