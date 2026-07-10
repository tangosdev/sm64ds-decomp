//cpp
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

struct Vector3 { int x, y, z; };

extern "C" {
int _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(void* self, Vector3* a, Vector3* out, int doStore);
void _ZN12WithMeshClsn13SetLimMovFlagEv(void* self);
}

extern s16 data_02082214[];

extern "C" int func_ov071_0211f8d0(char* self)
{
    Vector3* pos;
    Vector3 v;
    int zero;
    char* parent;
    int *pb0;
    int *py;
    int *pz;
    int s0, s1;
    u16 hang;
    int saved_x;
    int mul;
    int rnd;
    int one;
    Vector3* srcv;
    int adj;
    int y, z, x, y2;

    zero = 0;
    pb0 = (int *)(((int)self + 0xb0) & 0xFFFFFFFFFFFFFFFF);
    *pb0 = (*pb0) & 0xfff7fffe;

    parent = *(char **)(self + 0xd0);
    pos = (Vector3 *)(((int)self + 0x5c) & 0xFFFFFFFFFFFFFFFF);
    mul = 0x5a000;
    *(int *)(self + 0x98) = *(int *)(parent + 0x98) + 0x7000;
    *(int *)(self + 0xa8) = zero;

    parent = *(char **)(self + 0xd0);
    rnd = 0x800;
    *(s16 *)(self + 0x8e) = *(s16 *)(parent + 0x8e);
    py = (int *)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFF);
    pz = (int *)(((int)self + 0x64) & 0xFFFFFFFFFFFFFFFF);
    *(s16 *)(self + 0x94) = *(s16 *)(self + 0x8e);

    parent = *(char **)(self + 0xd0);
    one = 1;
    srcv = (Vector3 *)(((int)parent + 0x5c) & 0xFFFFFFFFFFFFFFFF);
    *(int *)(self + 0x5c) = srcv->x;
    *(int *)(self + 0x60) = srcv->y;
    *(int *)(self + 0x64) = srcv->z;

    saved_x = pos->x;
    hang = *(u16 *)(self + 0x8e);
    s0 = data_02082214[(hang >> 4) * 2];
    adj = (int)(((s64)s0 * mul + rnd) >> 12);
    pos->x = saved_x + adj;

    *py = *py + 0x50000;

    hang = *(u16 *)(self + 0x8e);
    s1 = data_02082214[(hang >> 4) * 2 + 1];
    adj = (int)(((s64)s1 * mul + rnd) >> 12);
    *pz = *pz + adj;

    parent = *(char **)(self + 0xd0);
    y = *(int *)(parent + 0x60);
    z = *(int *)(parent + 0x64);
    y2 = y + 0x50000;
    x = *(int *)(parent + 0x5c);
    v.x = x;
    v.y = y2;
    v.z = z;

    _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(self, &v, pos, one);

    *(int *)(self + 0xd0) = zero;
    _ZN12WithMeshClsn13SetLimMovFlagEv(self + 0x194);

    *(int *)(self + 0x39c) = 7;
    return 1;
}
