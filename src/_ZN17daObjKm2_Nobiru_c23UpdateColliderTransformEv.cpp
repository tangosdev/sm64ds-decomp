//cpp
// @symbol _ZN17daObjKm2_Nobiru_c23UpdateColliderTransformEv
#include "daObjKm2_Nobiru_c.h"

extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *, s32);

void daObjKm2_Nobiru_c::UpdateColliderTransform()
{
    Matrix4x3_FromRotationY(&mColliderTransform, mAngleY);
    mColliderTransform.t.x = mPosX;
    mColliderTransform.t.y = mPosY;
    mColliderTransform.t.z = mPosZ;
}
