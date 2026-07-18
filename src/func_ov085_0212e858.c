typedef unsigned int u32;
typedef int Fix12i;

struct Vec3 { int x, y, z; };
struct Mtx43 { int m[12]; };

extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(struct Mtx43* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern int _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *mtx, int fix, int t, u32 u);
extern void* _ZN5Actor13ClosestPlayerEv(void *self);
extern Fix12i Vec3_HorzDist(const struct Vec3* a, const struct Vec3* b);
extern short Vec3_HorzAngle(const struct Vec3* v0, const struct Vec3* v1);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(const struct Vec3* v, const struct Mtx43* m, struct Vec3* res);
extern void func_ov002_020e4374(char* c, int* p1, int* p2);

extern signed char data_0209f2f8;
extern struct Mtx43 data_020a0e68;

void func_ov085_0212e858(char *c)
{
    struct Vec3 p;
    struct Vec3 z1;
    struct Vec3 off;
    struct Vec3 pp;
    struct Vec3 t;
    char *pl;
    int p1, p2;
    signed char lvl;
    struct Vec3 *ps;

    Vec3_Asr(&t, (struct Vec3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(struct Mtx43*)(c + 0x12c) = data_020a0e68;

    Matrix4x3_FromTranslation(&data_020a0e68,
        *(int*)(c + 0x5c) >> 3,
        (*(int*)(c + 0x60) - 0x38000) >> 3,
        *(int*)(c + 0x64) >> 3);
    *(struct Mtx43*)(c + 0x240) = data_020a0e68;

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x1f0, c + 0x240, 0x46000, 0x258000, 0xf);

    pl = (char*)_ZN5Actor13ClosestPlayerEv(c);
    if (pl == 0) return;

    /* early load of level, zero stacks, then player pos copy interleaved with cmp */
    lvl = data_0209f2f8;
    p.x = 0;
    p.y = 0;
    p.z = 0;
    z1.x = 0;
    z1.y = 0;
    z1.z = 0;
    off.x = 0;
    off.y = 0;
    off.z = 0;
    ps = (struct Vec3*)(((long long)(int)(pl + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    pp.x = ps->x;
    pp.y = ps->y;
    pp.z = ps->z;
    if (lvl == 0x2f) {
        p.x = 0;
    } else {
        p.x = 0x1086000;
    }
    p.y = pp.y;
    p.z = pp.z;

    z1.z = Vec3_HorzDist(&pp, &p);
    {
        short ang = Vec3_HorzAngle(&pp, &p);
        Matrix4x3_FromRotationY(&data_020a0e68, ang);
    }
    MulVec3Mat4x3(&z1, &data_020a0e68, &off);

    p.x = p.x + off.x;
    p.y = pp.y;
    p.z = p.z + off.z;
    Matrix4x3_FromTranslation(&data_020a0e68, p.x >> 3, p.y >> 3, p.z >> 3);

    func_ov002_020e4374(pl, &p1, &p2);

    *(struct Mtx43*)(c + 0x270) = data_020a0e68;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c + 0x218, c + 0x270, p2, p1, 0xf);
}
