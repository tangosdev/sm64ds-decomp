// Vec3_HorzDist: horizontal distance between two Vector3 points (ignores Y)
#include "common.h"

extern Fix12i _ZN4cstd4sqrtEy(u64 val);

Fix12i Vec3_HorzDist(const Vector3 *a, const Vector3 *b) {
    Fix12i dx = a->x - b->x;
    Fix12i dz = a->z - b->z;
    long long sq = (long long)dx * dx;
    sq += (long long)dz * dz;
    return _ZN4cstd4sqrtEy((u64)sq);
}
