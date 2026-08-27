//cpp
// @symbol _ZN16daObjCtMecha04_c12UpdateShadowEv
/* Inferred descriptive name. The owned ShadowModel and its matrix are fixed by
 * the destructor, field accesses, and dActor_c::DropShadowScaleXYZ call. */
#include "daObjCtMecha04_c.h"

extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *matrix, s16 angle);
/* The real declaration takes three Fix12<int> values by value. This compiler
 * homes those class-typed arguments in the caller, unlike the cartridge call;
 * keep the measured register/stack ABI at this one boundary (notes 6az). */
extern "C" void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    int scaleX, int scaleY, int scaleZ, u32 opacity);

void daObjCtMecha04_c::UpdateShadow()
{
    int heightDiff = mPosY - mGroundY;
    int absHeightDiff = heightDiff < 0 ? -heightDiff : heightDiff;
    if (absHeightDiff > 0x7d0000)
        return;

    Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = (mGroundY + 0x1000) >> 3;
    mShadowMat.m[11] = mPosZ >> 3;

    int isLarge = (int)(actorID == 0x6f);
    if (isLarge != 0) {
        _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            this, &mShadowModel, &mShadowMat,
            0x1f4000, 0x32000, 0x3e8000, 0xf);
        return;
    }

    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &mShadowMat,
        0x1f4000, 0x32000, 0x320000, 0xf);
}
