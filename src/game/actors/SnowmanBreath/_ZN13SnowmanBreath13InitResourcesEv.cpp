//cpp
// @symbol _ZN13SnowmanBreath13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "SnowmanBreath.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void Vec3_Asr(Vector3 *d, const Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angY);
extern void InvMat4x3(Matrix4x3 *dst, const Matrix4x3 *src);
extern int data_ov002_0210da40[];
extern int data_ov002_0210d9a0[];
extern int data_ov002_0210d9c0[];
extern Matrix4x3 data_020a0e68;

typedef struct { int w[12]; } M48;
}

int SnowmanBreath::InitResources()
{
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da40);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a0);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9c0);
    mPosX = 0x3fa770;
    mPosY = 0xcb2000;
    mPosZ = 0x46988e;
    mAngleY = 0x5d30;
    Vector3 pos;
    Vec3_Asr(&pos, (Vector3 *)&mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    InvMat4x3(&data_020a0e68, &data_020a0e68);
    *(M48*)&mInvModelMat = *(M48*)&data_020a0e68;
    return 1;
}
