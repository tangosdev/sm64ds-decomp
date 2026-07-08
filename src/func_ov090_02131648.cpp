//cpp
// NONMATCHING: different op / idiom (div=87). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

struct C; typedef int (C::*PMF)();
struct C { char pad[0x420]; PMF *pp; };

struct RaycastLine {
    char pad[0x78];
};

extern "C" {
int RandomIntInternal(int *seed);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
void func_02012694(int a, void *p);
void _ZN11RaycastLineC1Ev(RaycastLine *self);
void Matrix4x3_FromRotationY(void *m, s16 angle);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
void MulVec3Mat4x3(const Vector3 *v, const void *m, Vector3 *out);
void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(RaycastLine *self, const Vector3 *a, const Vector3 *b, void *actor);
int _ZN11RaycastLine10DetectClsnEv(RaycastLine *self);
s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
void _Z14ApproachLinearRsss(s16 *cur, s16 target, s16 step);
int func_ov090_021314a0(void *c);
int func_ov090_02131e00(C *c, PMF *p);
void _ZN11RaycastLineD1Ev(RaycastLine *self);
}

extern int RNG_STATE;
extern char MATRIX_SCRATCH_PAPER[0x30];
extern char data_ov090_021344e4;
extern char data_ov090_02134504;

extern "C" int func_ov090_02131648(C *c)
{
    char *self = (char *)c;
    int dist;
    u32 rnd;
    int selfY;
    Vector3 a, b, in, out;
    RaycastLine rc;
    int angleSet;

    rnd = (u32)RandomIntInternal(&RNG_STATE) >> 8;
    dist = Vec3_Dist((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x374));

    if ((((*(u32 *)(self + 0x364)) << 4) >> 0x10 & 0xf) == 0) {
        func_02012694(0xfc, self + 0x74);
    }

    angleSet = 0;
    _ZN11RaycastLineC1Ev(&rc);

    a.x = 0; a.y = 0; a.z = 0;
    b.x = 0; b.y = 0; b.z = 0;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    a.x = *(int *)(self + 0x5c);
    selfY = *(int *)(self + 0x60);
    *(volatile int *)&a.y = selfY;
    a.z = *(int *)(self + 0x64);
    a.y = selfY + 0x64000;
    in.y = 0x64000;
    in.z = 0x1f4000;

    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(s16 *)(self + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, 0x2000);
    MulVec3Mat4x3(&in, &MATRIX_SCRATCH_PAPER, &out);

    b.x = a.x;
    b.x = a.x + out.x;
    b.y = a.y;
    b.z = a.z;
    b.y = a.y + out.y;
    b.z = a.z + out.z;

    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&rc, &a, &b, c);

    if (_ZN11RaycastLine10DetectClsnEv(&rc) == 0) {
        if (*(u8 *)(self + 0x3a0) == 0) {
            *(s16 *)(self + 0x39a) = Vec3_HorzAngle((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x374));
            angleSet = 1;
            *(s16 *)(self + 0x39a) += (0x1800 - ((rnd & 3) << 0xc));
            *(int *)(self + 0x5c) = *(int *)(self + 0x68);
            *(int *)(self + 0x60) = *(int *)(self + 0x6c);
            *(int *)(self + 0x64) = *(int *)(self + 0x70);
            *(u8 *)(self + 0x3a0) = 1;
        }
    } else {
        *(u8 *)(self + 0x3a0) = 0;
    }

    if (dist > 0x3c0000) {
        if (*(u8 *)(self + 0x39e) == 0 && *(u8 *)(self + 0x3a0) == 0) {
            *(s16 *)(self + 0x39a) = Vec3_HorzAngle((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x374));
            angleSet = 1;
            *(int *)(self + 0x5c) = *(int *)(self + 0x68);
            *(int *)(self + 0x60) = *(int *)(self + 0x6c);
            *(int *)(self + 0x64) = *(int *)(self + 0x70);
            *(u8 *)(self + 0x39e) = 1;
        }
    } else {
        *(u8 *)(self + 0x39e) = 0;
    }

    if (_ZNK12WithMeshClsn8IsOnWallEv(self + 0x150) != 0) {
        if (*(u8 *)(self + 0x39f) == 0 && *(u8 *)(self + 0x39e) == 0 && *(u8 *)(self + 0x3a0) == 0) {
            *(s16 *)(self + 0x39a) = Vec3_HorzAngle((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x374));
            angleSet = 1;
            *(s16 *)(self + 0x39a) += (0x1800 - ((rnd & 3) << 0xc));
            *(int *)(self + 0x5c) = *(int *)(self + 0x68);
            *(int *)(self + 0x60) = *(int *)(self + 0x6c);
            *(int *)(self + 0x64) = *(int *)(self + 0x70);
            *(u8 *)(self + 0x39f) = 1;
        }
    } else {
        *(u8 *)(self + 0x39f) = 0;
    }

    if (*(u16 *)(self + 0x398) == 0 && *(u8 *)(self + 0x39f) == 0 && *(u8 *)(self + 0x39e) == 0 && *(u8 *)(self + 0x3a0) == 0) {
        *(u16 *)(self + 0x398) = (u16)((rnd + 0x32) & 0x3f);
        *(s16 *)(self + 0x39a) = *(s16 *)(self + 0x39a) + (0x1800 - ((rnd & 3) << 0xc));
    }

    if (angleSet == 1) {
        *(u16 *)(self + 0x394) = 0xa;
        *(u16 *)(self + 0x398) = 0x32;
        *(u16 *)(self + 0x396) = 0x1e;
        *(int *)(self + 0x98) = 0x9000;
        *(int *)(self + 0x3a4) = 0x1000;
    }

    _Z14ApproachLinearRsss((s16 *)(self + 0x94), *(s16 *)(self + 0x39a), 0x500);

    if (*(u16 *)(self + 0x94) == 0 && *(u16 *)(self + 0x96) == 0) {
        *(int *)(self + 0x3a4) = 0x1000;
        *(int *)(self + 0x98) = 0x9000;
        if (func_ov090_021314a0(c) == 1) {
            *(u16 *)(self + 0x396) = 0x1e;
            *(u16 *)(self + 0x398) = 0x1e;
            *(int *)(self + 0x3a4) = 0x2000;
            *(int *)(self + 0x98) = 0xe000;
        }
    }

    if (((u32)((*(u32 *)(self + 0x364)) << 4) >> 0x10) >= 0x10) {
        *(int *)(self + 0x390) = *(int *)(self + 0x390) + 1;
    }

    if (*(int *)(self + 0x390) > 0x1e && *(u8 *)(self + 0x39e) == 0) {
        func_ov090_02131e00(c, (PMF *)&data_ov090_021344e4);
    }

    if (*(u8 *)(self + 0x39c) == 1) {
        func_ov090_02131e00(c, (PMF *)&data_ov090_02134504);
    }

    _ZN11RaycastLineD1Ev(&rc);
    return 1;
}
