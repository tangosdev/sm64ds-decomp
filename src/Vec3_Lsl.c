#include "common.h"

void Vec3_Lsl(Vector3 *d, const Vector3 *s, int sh) {
    fx32 a = s->x;
    fx32 b = s->y;
    fx32 c = s->z;
    d->x = a << sh;
    d->y = b << sh;
    d->z = c << sh;
}
