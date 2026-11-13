//cpp
#include "dCamera_c.h"
#include "dActor_c.h"



extern struct CamMode data_020874cc;

/* Camera state init: enter mode data_020874cc with mBaseAngleOffset set to
 * the yaw behind the target player minus the current mAngleY, so the camera
 * starts where it is; func_02005000 then eases the offset back to 0. */
int func_02005060(dCamera_c *cam) {
    cam->mMode = &data_020874cc;
    cam->mBaseAngleOffset = (short)(cam->mTargetPlayer->mAngleY + 0x8000) - cam->mAngleY;
    return 1;
}
