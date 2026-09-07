//cpp
#include "SpinningPlatform.h"

extern "C" void Matrix4x3_FromRotationY(void *m, short ang);
/* Fix12<int> by-value arguments are a measured mwccarm ABI wall: spelling this
 * as the class method homes the wrappers and no longer reproduces the ROM. */
extern "C" int _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern "C" short data_02082214[];

int SpinningPlatform::UpdateShadow()
{
    int d1 = mPosY - mFloorPosY;
    int ad = d1 < 0 ? -d1 : d1;
    if (ad > 0x7d0000) return ad;
    int r2 = ad + 0x350000;
    int half = ad / 2;
    if (r2 < 0x350000) r2 = 0x350000;
    mClipOffsetY = -half;
    mClipRadius = (int)(r2 + ((unsigned)r2 >> 31)) >> 4;
    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = mFloorPosY >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    int idx = (unsigned short)mAngleX >> 4;
    int s = data_02082214[(idx << 1) + 1];
    int sa = s < 0 ? -s : s;
    int scaled = (int)(((long long)sa * 0x26c000 + 0x800) >> 12);
    return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &mShadowMat, 0x12c000, 0x32000,
        scaled + 0x50000, 0xf);
}
