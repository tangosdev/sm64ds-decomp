#include "common.h"

int Vec3_Equal(const Vector3 *a, const Vector3 *b) {
    if (a->x == b->x && a->y == b->y && a->z == b->z) return 1;
    return 0;
}
