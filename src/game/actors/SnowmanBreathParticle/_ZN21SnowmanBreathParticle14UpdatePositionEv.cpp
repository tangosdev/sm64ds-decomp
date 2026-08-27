//cpp
// @symbol _ZN21SnowmanBreathParticle14UpdatePositionEv
#include "SnowmanBreath.h"

extern "C" {
extern Matrix4x3 data_020a0e68;
void Matrix4x3_FromRotationY(Matrix4x3 *, s16);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *, s16);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *, s32, s32, s32);
void AddVec3(const Vector3 *, const Vector3 *, Vector3 *);
}

void SnowmanBreathParticle::UpdatePosition()
{
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0, 0x50000);
    Vector3 offset;
    offset = data_020a0e68.t;
    AddVec3(&mPos, &offset, &mPos);
}
