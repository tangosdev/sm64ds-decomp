//cpp
// @symbol _ZN14daObjC1Peach_c20UpdateModelTransformEv
#include "daObjC1Peach_c.h"

extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *, s32);

void daObjC1Peach_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}
