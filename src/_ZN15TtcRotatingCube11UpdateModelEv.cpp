//cpp
// @symbol _ZN15TtcRotatingCube11UpdateModelEv
#include "TtcRotatingCube.h"

extern "C" void Matrix4x3_FromRotationZXYExt(void* m, int x, int y, int z);
extern "C" void Matrix4x3_FromRotationY(void* m, int angle);

void TtcRotatingCube::UpdateModel()
{
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, 0, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = (mPosY + mOffsetY) >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
    Matrix4x3_FromRotationY(&mRotatingModel.mat4x3, mAngleY);
    mRotatingModel.mat4x3.m[9] = mModel.mat4x3.m[9];
    mRotatingModel.mat4x3.m[10] = mPosY >> 3;
    mRotatingModel.mat4x3.m[11] = mModel.mat4x3.m[11];
}
