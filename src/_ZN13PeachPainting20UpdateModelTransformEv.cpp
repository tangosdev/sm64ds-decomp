//cpp
// @symbol _ZN13PeachPainting20UpdateModelTransformEv
#include "PeachPainting.h"

extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *, s32);

void PeachPainting::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}
