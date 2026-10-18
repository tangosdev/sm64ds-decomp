#include "common.h"

void Vec3_Lsl(Vector3 *d, const Vector3 *s, int sh) {
    Fix12i a = s->x;
    Fix12i b = s->y;
    Fix12i c = s->z;
    d->x = a << sh;
    d->y = b << sh;
    d->z = c << sh;
}
