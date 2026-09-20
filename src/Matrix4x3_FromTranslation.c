#include "common.h"
void Matrix4x3_FromTranslation(Matrix4x3* m, int x, int y, int z) {
    m->m[0] = 0x1000;
    m->m[1] = 0;
    m->m[2] = 0;
    m->m[3] = 0;
    m->m[4] = 0x1000;
    m->m[5] = 0;
    m->m[6] = 0;
    m->m[7] = 0;
    m->m[8] = 0x1000;
    m->m[9] = x;
    m->m[10] = y;
    m->m[11] = z;
}
