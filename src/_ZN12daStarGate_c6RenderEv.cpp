//cpp
// @symbol _ZN8StarDoor6RenderEv

#include "StarDoor.h"

extern "C" {
u8 IsAreaShowing(int areaID);
void Matrix4x3_FromRotationY(Matrix4x3 *matrix, int angle);
}

int StarDoor::Render()
{
    if (IsAreaShowing((s8)mAngleX) != 0 ||
        IsAreaShowing((s8)mAngleZ) != 0) {
        Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
        mModel.mat4x3.t.x = (mPosX + unk_0a4) >> 3;
        mModel.mat4x3.t.y = (mPosY + mVertSpeed) >> 3;
        mModel.mat4x3.t.z = (mPosZ + unk_0ac) >> 3;
        mModel.Render(0);

        Matrix4x3_FromRotationY(&mModel.mat4x3, (s16)(mAngleY + 0x8000));
        mModel.mat4x3.t.x = (mPosX - unk_0a4) >> 3;
        mModel.mat4x3.t.y = (mPosY - mVertSpeed) >> 3;
        mModel.mat4x3.t.z = (mPosZ - unk_0ac) >> 3;
        mModel.Render(0);
    }

    return 1;
}
