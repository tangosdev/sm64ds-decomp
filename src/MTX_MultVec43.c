#include "common.h"

void MTX_MultVec43(Vector3 *v, Matrix4x3 *m, Vector3 *dst) {
    int x = v->x;
    int y = v->y;
    int z = v->z;

    dst->x = (((long long)x * m->m[0] + (long long)y * m->m[3] + (long long)z * m->m[6]) >> 12);
    dst->x += m->m[9];
    dst->y = (((long long)x * m->m[1] + (long long)y * m->m[4] + (long long)z * m->m[7]) >> 12);
    dst->y += m->m[10];
    dst->z = (((long long)x * m->m[2] + (long long)y * m->m[5] + (long long)z * m->m[8]) >> 12);
    dst->z += m->m[11];
}
