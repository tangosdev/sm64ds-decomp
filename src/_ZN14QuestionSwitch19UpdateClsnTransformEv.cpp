//cpp
// @symbol _ZN19daObjHatenaSwitch_c19UpdateClsnTransformEv
/* Inferred descriptive name. Both callers are daObjHatenaSwitch_c methods and the
 * body rebuilds only this object's active moving-mesh transform. */
#include "QuestionSwitch.h"

struct daObjHatenaSwitch_cMatrixCopy {
    s32 words[12];
};

void daObjHatenaSwitch_c::UpdateClsnTransform()
{
    *(daObjHatenaSwitch_cMatrixCopy *)&mClsnMat =
        *(daObjHatenaSwitch_cMatrixCopy *)&mModelAnim.mat4x3;
    mClsnMat.t.x = mPosX;
    mClsnMat.t.y = mPosY;
    mClsnMat.t.z = mPosZ;
    mActiveMeshCollider->Transform(mClsnMat, mAngleY);
}
