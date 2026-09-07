//cpp
// @symbol _ZN13SnowmanBreath15IsPlayerInRangeEv
#include "SnowmanBreath.h"

extern "C" {
void Vec3_Asr(Vector3 *, const Vector3 *, int);
void MulVec3Mat4x3(const Vector3 *, const Matrix4x3 *, Vector3 *);
void Vec3_LslInPlace(Vector3 *, int);
extern Matrix4x3 data_020a0e68;
typedef struct { s32 words[12]; } MatrixWords;
}

int SnowmanBreath::IsPlayerInRange()
{
    Player *player = ClosestPlayer();
    mTalkPlayer = player;

    Vector3 playerPos;
    Vec3_Asr(&playerPos, (Vector3 *)((char *)player + 0x5c), 3);
    *(MatrixWords *)&data_020a0e68 = *(MatrixWords *)&mInvModelMat;

    Vector3 localPos;
    MulVec3Mat4x3(&playerPos, &data_020a0e68, &localPos);
    Vec3_LslInPlace(&localPos, 3);

    if (localPos.x < -0xe1000)
        goto fail;
    if (localPos.x > 0xe1000)
        goto fail;
    if (localPos.y < -0x5000)
        goto fail;
    if (localPos.y > 0x300000)
        goto fail;
    if (localPos.z < -0x430000)
        goto fail;
    if (localPos.z > 0x430000)
        goto fail;
    return 1;
fail:
    return 0;
}
