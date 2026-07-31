#include "types.h"
// @symbol func_ov072_02121368
// @emits daBgSnwmn_c_Kill
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_WithMeshClsn.h"
/* recovered: shared common types, renamed to Class_Method */
/* daBgSnwmn_c::Kill - recovered from vtable slot identity */
extern s16 data_02082214[];
extern int _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(void *self, struct Vector3 *a, struct Vector3 *out, int doStore);

#define M(p) ((s64)(int)(p))

int daBgSnwmn_c_Kill(char *self)
{
    struct Vector3 v;
    char *other;
    int *pos;
    int *py;
    int *pz;
    s16 s;
    u16 ang;
    int mul;
    int rnd;
    int flag;
    struct Vector3 *vp;
    int y;
    int z;
    int y2;
    int x;

    *(int *)(int)M(self + 0xb0) &= ~0x80000;
    *(int *)(self + 0x98) = 0xa000;
    *(int *)(self + 0xa8) = 0;

    other = *(char **)(self + 0xd0);
    pos = (int *)(int)M(self + 0x5c);
    s = *(s16 *)(other + 0x8e);
    mul = 0x50000;
    rnd = 0x800;
    *(s16 *)(self + 0x8e) = s;
    s = *(s16 *)(self + 0x8e);
    py = (int *)(int)M(self + 0x60);
    pz = (int *)(int)M(self + 0x64);
    *(s16 *)(self + 0x94) = s;

    other = *(char **)(self + 0xd0);
    {
        int *op = (int *)(int)M(other + 0x5c);
        int t0 = *op;
        vp = &v;
        *(int *)(self + 0x5c) = t0;
        int t1 = op[1];
        flag = 1;
        *(int *)(self + 0x60) = t1;
        int t2 = op[2];
        *(int *)(self + 0x64) = t2;
    }

    ang = *(u16 *)(self + 0x8e);
    s = data_02082214[(ang >> 4) * 2];
    *pos = *pos + (int)(((s64)s * mul + rnd) >> 12);
    *py = *py + mul;

    ang = *(u16 *)(self + 0x8e);
    s = data_02082214[(ang >> 4) * 2 + 1];
    *pz = *pz + (int)(((s64)s * mul + rnd) >> 12);

    other = *(char **)(self + 0xd0);
    y = *(int *)(other + 0x60);
    z = *(int *)(other + 0x64);
    y2 = y + mul;
    x = *(int *)(other + 0x5c);
    v.x = x;
    v.y = y2;
    v.z = z;
    _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(self, vp, (struct Vector3 *)pos, flag);

    *(int *)(self + 0xd0) = 0;
    _ZN12WithMeshClsn13SetLimMovFlagEv(self + 0x194);
    *(int *)(self + 0x368) = 5;
    return 1;
}
