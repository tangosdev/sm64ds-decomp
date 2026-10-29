//cpp
#include "common.h"
#include "cstd.h"

extern "C" {

extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern Fix12i Vec3_HorzLen(const Vector3 *v);

s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0)
{
    Vector3 temp;
    Vec3_Sub(&temp, v1, v0);
    Fix12i horzLen = Vec3_HorzLen(&temp);
    return cstd::atan2(temp.y, horzLen);
}
}
