//cpp
// @symbol _ZN10SlidingBox11UpdateModelEv
#include "SlidingBox.h"

extern "C" void Matrix4x3_FromRotationXYZExt(
    Matrix4x3 *matrix, s32 angleX, s32 angleY, s32 angleZ);

void SlidingBox::UpdateModel()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}
