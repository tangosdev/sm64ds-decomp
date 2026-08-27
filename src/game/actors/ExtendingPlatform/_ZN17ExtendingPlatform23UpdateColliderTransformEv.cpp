//cpp
// @symbol _ZN17ExtendingPlatform23UpdateColliderTransformEv
#include "ExtendingPlatform.h"

extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *, s32);

void ExtendingPlatform::UpdateColliderTransform()
{
    Matrix4x3_FromRotationY(&mColliderTransform, mAngleY);
    mColliderTransform.t.x = mPosX;
    mColliderTransform.t.y = mPosY;
    mColliderTransform.t.z = mPosZ;
}
