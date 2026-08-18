// @symbol func_ov010_0211184c
/* recovered: shared common types */
#include "common.h"
extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern short Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern short data_02082214[];
void func_ov010_0211184c(char* c, char* arg2) {
    char* target;
    unsigned int id;
    struct Vector3 va, vt;
    struct Vector3 *sa, *st;
    short ang;
    short ang2;
    int dist;
    int idx;
    short tv;
    int d;
    int b;
    if (*(int*)(c+0x3a0) != 0) return;
    if (*(short*)(c+0x90) < -0x3000) return;
    id = *(unsigned int*)(c+0x3ac);
    if (id == 0) return;
    target = _ZN8dActor_c10FindWithIDEj(id);
    if (target == 0) return;
    b = (int)(*(unsigned short*)(arg2+0xc) == 0xbf);
    if (b == 0) return;
    sa = (struct Vector3*)(((int)arg2 + 0x5c));
    st = (struct Vector3*)(((int)target + 0x5c));
    va.x = sa->x;
    va.y = sa->y;
    va.z = sa->z;
    vt.x = st->x;
    vt.y = st->y;
    vt.z = st->z;
    ang = Vec3_HorzAngle(&vt, &va);
    ang2 = (short)(ang + *(short*)(target+0x8e));
    dist = Vec3_HorzDist(&vt, &va);
    idx = (unsigned short)ang2 >> 4;
    tv = data_02082214[idx*2];
    d = (int)(((long long)dist * tv + 0x800) >> 12);
    if (d < 0x10e000) *(unsigned char*)(target+0x3aa) = 1;
}
