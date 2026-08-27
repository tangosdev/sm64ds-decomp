//cpp
// @symbol _ZN15TtcRotatingCube12UpdateShadowEv
#include "TtcRotatingCube.h"

extern "C" void Matrix4x3_FromRotationY(void *m, short ang);
extern "C" int _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern short data_02082214[];

int TtcRotatingCube::UpdateShadow() {
    int d, b;
    int bval = mFloorY;
    int aval = mPosY;
    int diff = aval - bval;
    if (diff < 0) diff = -diff;
    if (diff > 0x9c4000) return 0x9c4000;
    b = 0x32000;
    d = 0x190000;
    if (mUnevenGround) { b = 0x17c000; d = b; }
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = (mFloorY + 0x1000) >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    {
        int idx = (unsigned short)(short)(mAngleZ << 1) >> 4;
        int s = data_02082214[idx << 1];
        int sa = s < 0 ? -s : s;
        int r3 = (int)(((long long)sa * 0x64000 + 0x800) >> 12);
        return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            this, &mShadowModel, &mShadowMat, d + r3, b, d, 0xf);
    }
}
