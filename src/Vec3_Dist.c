// Vec3_Dist: 3D distance between two Vector3 points
#include "common.h"

extern Fix12i _ZN4cstd4sqrtEy(u64 val);

Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b) {
    Fix12i dx = a->x - b->x;
    Fix12i ay = a->y;
    long long sq = (long long)dx * dx;
    Fix12i by = b->y;
    Fix12i az = a->z;
    ay -= by;
    Fix12i bz = b->z;
    sq += (long long)ay * ay;
    ay = az - bz;
    sq += (long long)ay * ay;
    return _ZN4cstd4sqrtEy((u64)sq);
}
