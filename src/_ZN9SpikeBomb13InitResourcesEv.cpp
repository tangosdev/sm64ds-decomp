//cpp
// @symbol _ZN9SpikeBomb13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SpikeBomb.h"

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *f, int a, int b);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *thiz, void *actor, const Vector3 &v, int radius, int height, unsigned a, unsigned b);
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern int Vec3_HorzLen(const Vector3 *v);
}

int SpikeBomb::InitResources()
{
    Vector3 v;
    Vector3 z;
    void *file;
    int *p178;
    int t;

    file = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov060_0211b1c4);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, file, 1, -1);
    v.x = 0;
    v.y = -0x96000;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        ((char *)this) + 0x124, ((char *)this), v, 0x96000, 0x12c000, 0x204004, 0);
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mOpacity = 0xff;
    z.x = 0;
    z.y = 0;
    z.z = 0;
    Vec3_HorzAngle(&z, (const Vector3 *)((char *)&mPosX));
    p178 = (int *)((char *)&mHomePosY);
    mHomeYOffset = 0x2ee000;
    t = mPosX;
    /* materialize r0 = ((char *)this)+0x5c between load and store */
    {
        Vector3 *pos = (Vector3 *)((char *)&mPosX);
        (void)pos;
    }
    mHomePosX = t;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    *p178 = *p178 + (mHomeYOffset >> 3);
    mHomeHorzDist = Vec3_HorzLen((const Vector3 *)((char *)&mPosX));
    mStateIndex = 0;
    mSlotIndex = AddSpikeBomb(((char *)this));
    return 1;
}
