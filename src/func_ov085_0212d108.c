// NONMATCHING: different op / idiom (div=29). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;
typedef int s32;

typedef struct { s32 x, y, z; } Vector3;
typedef struct { Vector3 c0, c1, c2, c3; } Matrix4x3;

typedef struct C {
    char pad0[0x5c];
    Vector3 pos;        /* 0x5c (y@0x60) */
    char pad68[0x8c-0x68];
    s16 field_8c;       /* 0x8c */
    char pad8e[0x92-0x8e];
    s16 field_92;       /* 0x92 */
    s16 field_94;       /* 0x94 */
    char pad96[0xa4-0x96];
    s32 field_a4;       /* 0xa4 */
    s32 field_a8;       /* 0xa8 */
    s32 field_ac;       /* 0xac */
    char padb0[0x100-0xb0];
    unsigned short field_100; /* 0x100 */
    char pad102[0x18c-0x102];
    void *field_18c;    /* 0x18c */
} C;

extern C *_ZN5Actor13ClosestPlayerEv(void);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *mF, s16 angX);
extern void MulVec3Mat4x3(const Vector3 *v, const Matrix4x3 *m, Vector3 *res);
extern void ApproachLinear(int *a, int b, int c);
extern void func_ov085_0212d268(C *c, void *p);
extern Matrix4x3 MATRIX_SCRATCH_PAPER;
extern char data_ov085_0213072c;

int func_ov085_0212d108(C *c) {
    Vector3 v;
    Vector3 out;
    Vector3 pp;
    C *player;
    s16 *ap;

    ap = &c->field_8c;
    *ap += 0x1000;
    if (c->field_100) return 1;
    player = _ZN5Actor13ClosestPlayerEv();
    if (!player) return 1;

    v.x = 0;
    v.y = 0;
    v.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    pp = player->pos;
    c->field_94 = Vec3_HorzAngle(&c->pos, &pp);
    c->field_92 = Vec3_VertAngle(&c->pos, &pp);
    v.z = 0x14000;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, c->field_94);
    Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, c->field_92);
    MulVec3Mat4x3(&v, &MATRIX_SCRATCH_PAPER, &out);
    ApproachLinear(&c->field_a4, out.x, 0x1000);
    ApproachLinear(&c->field_a8, out.y, 0x1000);
    ApproachLinear(&c->field_ac, out.z, 0x1000);
    if (pp.y + 0x64000 > c->pos.y) {
        c->field_18c = player;
        func_ov085_0212d268(c, &data_ov085_0213072c);
    }
    return 1;
}
