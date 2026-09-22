//cpp
// @symbol _ZN15TtcRotatingCube10UpdateClsnEv
#include "TtcRotatingCube.h"
extern "C" {
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(dBgW_KcMbg*, Matrix4x3&, short);
}

void TtcRotatingCube::UpdateClsn()
{
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY + mOffsetY;
    mClsnMat.m[11] = mPosZ;
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(&mMeshCollider, mClsnMat, mAngleY);
}
