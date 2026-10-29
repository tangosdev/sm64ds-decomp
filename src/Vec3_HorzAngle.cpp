//cpp
#include "common.h"
#include "cstd.h"

extern "C" {

s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1) {
    Fix12i dx = v1->x - v0->x;
    Fix12i dz = v1->z - v0->z;
    return cstd::atan2(dx, dz);
}
}
