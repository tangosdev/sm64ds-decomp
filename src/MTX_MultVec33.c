#include "types.h"
#include "math/Matrix.h"

void MTX_MultVec33(const Vector3 *in, const Matrix3x3 *m, Vector3 *out) {
    int x = in->x;
    int y = in->y;
    int z = in->z;

    out->x = (((s64)x * m->m[0] + (s64)y * m->m[3] + (s64)z * m->m[6]) >> 12);
    out->y = (((s64)x * m->m[1] + (s64)y * m->m[4] + (s64)z * m->m[7]) >> 12);
    out->z = (((s64)x * m->m[2] + (s64)y * m->m[5] + (s64)z * m->m[8]) >> 12);
}
