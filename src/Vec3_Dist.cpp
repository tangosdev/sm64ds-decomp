//cpp
// Vec3_Dist: 3D distance between two Vector3 points
#include "common.h"
#include "cstd.h"

extern "C" {

fx32 Vec3_Dist(const Vector3 *a, const Vector3 *b) {
    fx32 dx = a->x - b->x;
    fx32 ay = a->y;
    long long sq = (long long)dx * dx;
    fx32 by = b->y;
    fx32 az = a->z;
    ay -= by;
    fx32 bz = b->z;
    sq += (long long)ay * ay;
    ay = az - bz;
    sq += (long long)ay * ay;
    return cstd::sqrt((u64)sq);
}
}
