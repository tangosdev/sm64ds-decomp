#include "common.h"

extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);

s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1) {
    Fix12i dx = v1->x - v0->x;
    Fix12i dz = v1->z - v0->z;
    return _ZN4cstd5atan2E5Fix12IiES1_(dx, dz);
}
