//cpp
#include "common.h"
#include "cstd.h"
#include "math/CalcLib.h"

extern "C" {

s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0)
{
    Vector3 diff;
    Vec3_Sub(&diff, v1, v0);
    fx32 horzLen = Vec3_HorzLen(&diff);
    return cstd::atan2(diff.y, horzLen);
}
}
