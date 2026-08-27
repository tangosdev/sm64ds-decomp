//cpp
// @symbol _ZN10dBgActor_c21UpdateModelPosAndRotYEv
/* recovered: named members + shared header, real C++ method
 *
 * `((char *)this) + 0xf0` and the three unk_114/118/11c writes were the same object
 * reached two different ways: 0xf0 is mModel.mat4x3, and 0x114/0x118/0x11c are its
 * translation row, m[9]..m[11]. Build the rotation into the model's matrix, then
 * drop the position into it -- at 1/8 scale, which is the model-space unit.
 */
#include "dBgActor_c.h"

extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *mat, int angleY);

void dBgActor_c::UpdateModelPosAndRotY()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9]  = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}
