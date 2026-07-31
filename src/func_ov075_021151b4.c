// @symbol func_ov075_021151b4
/* recovered: shared common types */
#include "common.h"
extern void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern struct Matrix4x3 data_020a0e68;

struct Entry { char pad[0x118]; int x, y, z; char rest[0x34]; };

void func_ov075_021151b4(char *c, int idx)
{
    struct Entry *e = ((struct Entry *)(c + 0x920)) + idx;
    struct Vector3 *src = (struct Vector3 *)(((long long)(int)((char *)e + 0x118)));
    struct Vector3 v, out;
    int y;

    v.x = src->x;
    y = src->y;
    v.y = y;
    v.z = src->z;
    v.y = y + 0x32000;
    Vec3_Asr(&out, &v, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, out.x, out.y, out.z);
    *(struct Matrix4x3 *)(c + 0x8d8) = data_020a0e68;
}
