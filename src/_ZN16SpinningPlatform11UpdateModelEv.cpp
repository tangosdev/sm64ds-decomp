//cpp
#include "SpinningPlatform.h"

extern "C" void Matrix4x3_FromRotationXYZExt(void *, int, int, int);

void SpinningPlatform::UpdateModel()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}
