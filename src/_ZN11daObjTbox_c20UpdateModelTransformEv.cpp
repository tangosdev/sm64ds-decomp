//cpp
// @symbol _ZN11daObjTbox_c20UpdateModelTransformEv
#include "daObjTbox_c.h"

extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *, s32);

void daObjTbox_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.t.x = mPosX >> 3;
    mModelAnim.mat4x3.t.y = mPosY >> 3;
    mModelAnim.mat4x3.t.z = mPosZ >> 3;
}
