#include "common.h"
void Matrix4x3_FromTranslation(Matrix4x3* m, int x, int y, int z) {
    m->r.m[0][0] = 0x1000;
    m->r.m[0][1] = 0;
    m->r.m[0][2] = 0;
    m->r.m[1][0] = 0;
    m->r.m[1][1] = 0x1000;
    m->r.m[1][2] = 0;
    m->r.m[2][0] = 0;
    m->r.m[2][1] = 0;
    m->r.m[2][2] = 0x1000;
    m->t.x = x;
    m->t.y = y;
    m->t.z = z;
}
