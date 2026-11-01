#include "types.h"

void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b) {
    int ax = a->x, bx = b->x;
    int az = a->z, bz = b->z;
    int ay = a->y, by = b->y;
    out->x = ax - bx;
    out->y = ay - by;
    out->z = az - bz;
}
