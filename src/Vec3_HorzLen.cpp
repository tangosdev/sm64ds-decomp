//cpp
// Vec3_HorzLen: horizontal length of a Vector3 (ignores Y)
#include "common.h"
#include "cstd.h"

extern "C" {

fx32 Vec3_HorzLen(const Vector3 *v) {
    long long sq = (long long)v->x * v->x;
    sq += (long long)v->z * v->z;
    return cstd::sqrt(sq);
}
}
