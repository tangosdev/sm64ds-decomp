//cpp
#include "dCamera_c.h"
#include "dActor_c.h"

extern struct CamMode data_020874cc;

/* Camera state init: enter mode data_020874cc with mBaseAngleOffset set to
 * the yaw behind the target player minus the current mAngleY, so the camera
 * starts where it is; func_02005000 then eases the offset back to 0. */
int dCamera_c::func_02005060() {
    this->mMode = &data_020874cc;
    this->mBaseAngleOffset = (short)(this->mTargetPlayer->mAngle.y + 0x8000) - this->mAngleY;
    return 1;
}
