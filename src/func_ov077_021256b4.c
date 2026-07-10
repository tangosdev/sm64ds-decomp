typedef long long s64;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
struct Vector3 { int x, y, z; };
extern s16 data_02082214[];
extern int _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(void *self, struct Vector3 *a, struct Vector3 *out, int doStore);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void *self);
#define LA(p) ((int)(((s64)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))
int func_ov077_021256b4(char *o)
{
    char *d0;
    int *a5c;
    int *a60;
    int *a64;
    int *src;
    int k;
    s16 s;
    s16 cval;
    s16 ang;
    struct Vector3 v;
    int one;
    int y;
    int z;
    int x;

    *(int *)LA(o + 0xb0) &= ~0x80000;
    *(int *)(o + 0x98) = 0xa000;
    *(int *)(o + 0xa8) = 0;

    d0 = *(char **)(o + 0xd0);
    a5c = (int *)LA(o + 0x5c);
    ang = *(s16 *)(d0 + 0x8e);
    *(s16 *)(o + 0x8c) = 0;
    *(s16 *)(o + 0x8e) = ang;
    *(s16 *)(o + 0x90) = 0;

    *(s16 *)(o + 0x94) = *(s16 *)(o + 0x8e);
    a60 = (int *)LA(o + 0x60);
    a64 = (int *)LA(o + 0x64);

    d0 = *(char **)(o + 0xd0);
    src = (int *)LA(d0 + 0x5c);
    one = 1;
    *(int *)(o + 0x5c) = src[0];
    *(int *)(o + 0x60) = src[1];
    *(int *)(o + 0x64) = src[2];

    k = ((int)*(u16 *)(o + 0x8e)) >> 4;
    s = data_02082214[k * 2];
    *a5c = *a5c + (int)(((s64)s * 0x3c000 + 0x800) >> 12);
    *a60 = *a60 + 0x85000;
    k = ((int)*(u16 *)(o + 0x8e)) >> 4;
    cval = data_02082214[k * 2 + 1];
    *a64 = *a64 + (int)(((s64)cval * 0x3c000 + 0x800) >> 12);

    d0 = *(char **)(o + 0xd0);

    {
        int ty = *(int *)(d0 + 0x60);
        int tz = *(int *)(d0 + 0x64);
        int ty2 = ty + 0x50000;
        int tx = *(int *)(d0 + 0x5c);
        v.x = tx; v.y = ty2; v.z = tz;
    }

    _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(o, &v, (struct Vector3 *)(o + 0x5c), one);

    *(int *)(o + 0xd0) = 0;
    _ZN12WithMeshClsn13SetLimMovFlagEv(o + 0x1e4);

    *(int *)(o + 0x3d8) = 4;
    return 1;
}
