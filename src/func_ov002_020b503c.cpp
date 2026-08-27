//cpp
// @symbol _ZN14QuestionSwitch19UpdateClsnTransformEv
/* Inferred descriptive name. Both callers are QuestionSwitch methods and the
 * body rebuilds only this object's active moving-mesh transform. */
#include "QuestionSwitch.h"

struct QuestionSwitchMatrixCopy {
    s32 words[12];
};

void QuestionSwitch::UpdateClsnTransform()
{
    *(QuestionSwitchMatrixCopy *)&mClsnMat =
        *(QuestionSwitchMatrixCopy *)&mModelAnim.mat4x3;
    mClsnMat.t.x = mPosX;
    mClsnMat.t.y = mPosY;
    mClsnMat.t.z = mPosZ;
    mActiveMeshCollider->Transform(mClsnMat, mAngleY);
}
