
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

struct Vector3 { int x, y, z; };
typedef struct P2 { int a, b; } P2;
typedef struct ClsnResult { void *vtb; int s0, s1, s2, s3, s4; u16 f, g; int h, i, j; } ClsnResult;
typedef struct Vec3_16 { s16 x, y, z; } Vec3_16;

extern void _ZN11RaycastLineC1Ev(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern int _ZNK6Player14GetBodyModelIDEjb(void *thiz, unsigned int a, int b);
extern void MulVec3Mat4x3(void *v, void *m, void *dst);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, void *a, void *b, void *act);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN11RaycastLine10GetClsnPosEv(void *res, void *self);
extern unsigned int _ZNK10ClsnResult9GetClsnIDEv(void *r);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN10ClsnResultD1Ev(void *r);
extern int func_02053274(int *a, int *b);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *thiz, void *out);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int x, int y, int z, const void *v, void *cb);

extern short data_02082214[];
extern void *data_02099368[];

int func_ov002_020d6dac(char *self)
{
    Vec3_16 dir;
    struct Vector3 p1;
    struct Vector3 p2;
    ClsnResult res;
    struct Vector3 clsnPos;
    struct Vector3 normal;
    char rl[0x78];
    int *d;
    char *actor;
    s16 ang;
    int t;
    int idx;

    _ZN11RaycastLineC1Ev(rl);

    {
        u8 b = *(u8 *)(self + 0x714);
        int model;
        char *m1 = (char *)(*(int *)(*(int *)(self + ((b << 2) + 3) * 4 + 0x154) + 0x14));
        model = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m1 + 0x24, (char *)(*(int *)(self + model * 4 + 0xdc)) + 0x1c, &p1);
        Vec3_MulScalarInPlace((int *)&p1, 0x8000);
    }

    {
        u8 b2 = *(u8 *)(self + 0x714);
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        int model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        void *a0, *a1, *a2;
        a1 = (char *)(*(int *)(self + model2 * 4 + 0xdc));
        a0 = (char *)m2i + 0x24;
        a1 = (char *)a1 + 0x1c;
        a2 = &p2;
        MulVec3Mat4x3(a0, a1, a2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &p1, &p2, self);

    if (_ZN11RaycastLine10DetectClsnEv(rl) == 0) {
        _ZN11RaycastLineD1Ev(rl);
        return 0;
    }

    d = &res.s0;
    *(P2 *)d = *(P2 *)(rl + 0x14);
    d[2] = *(int *)(rl + 0x1c);
    d[3] = *(int *)(rl + 0x20);
    d[4] = *(int *)(rl + 0x24);
    res.vtb = data_02099368;
    res.f = *(u16 *)(rl + 0x28);
    res.g = *(u16 *)(rl + 0x2a);
    res.h = *(int *)(rl + 0x2c);
    res.i = *(int *)(rl + 0x30);
    res.j = *(int *)(rl + 0x34);

    actor = _ZN5Actor10FindWithIDEj(_ZNK10ClsnResult9GetClsnIDEv(&res));
    if (actor != 0 && (t = (int)(*(u16 *)(actor + 0xc) == 0xc2)) != 0) {
        _ZN10ClsnResultD1Ev(&res);
        _ZN11RaycastLineD1Ev(rl);
        return 0;
    }

    _ZN11RaycastLine10GetClsnPosEv(&clsnPos, rl);
    *(int *)(self + 0x654) = func_02053274((int *)&p1, (int *)&clsnPos);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rl + 0x14, &normal);
    ang = _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z);
    idx = (u16)ang >> 4;
    dir.x = data_02082214[idx * 2];
    dir.y = 0;
    dir.z = data_02082214[idx * 2 + 1];
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0x13e, clsnPos.x, clsnPos.y, clsnPos.z, &dir, 0);
    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        0, 0x13f, clsnPos.x, clsnPos.y, clsnPos.z, &dir, 0);

    _ZN10ClsnResultD1Ev(&res);
    _ZN11RaycastLineD1Ev(rl);
    return 1;
}
