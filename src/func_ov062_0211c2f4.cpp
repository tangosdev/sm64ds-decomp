//cpp
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

struct PathPtr {
    int a;
    int b;
    PathPtr();
    void FromID(u32 id);
    void GetNode(Vector3 &out, u32 idx) const;
};

extern "C" {
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern int ApproachAngle(void *p, int target, int a, int b, int c);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern int LenVec3(Vector3 *v);
extern int func_ov062_0211b3ac(void *c);
extern int RandomIntInternal(int *seed);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(Vector3 *in, void *m, void *out);
extern void func_ov062_0211c658(void *c, void *p);
extern int AngleDiff(int a, int b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(void *out, Vector3 *in, int s);
extern void SubVec3(void *a, void *b, void *c);

extern s8 LEVEL_ID;
extern int RNG_STATE;
extern int MATRIX_SCRATCH_PAPER;
extern int data_ov062_0211e17c;
}

extern "C" int func_ov062_0211c2f4(char *self) {
    PathPtr path;
    int zero[6];
    Vector3 node;
    Vector3 node2;
    Vector3 diff;
    Vector3 scaled;
    int len;
    u32 idx;

    path.FromID(*(u32 *)(self + 0x464));
    zero[0] = 0; zero[1] = 0; zero[2] = 0; zero[3] = 0; zero[4] = 0; zero[5] = 0;
    path.GetNode(node, *(u32 *)(self + 0x474));
    ApproachAngle((void *)(self + 0x94), Vec3_HorzAngle(self + 0x5c, &node), 0xa, 0x200, 0x100);
    ApproachAngle((void *)(self + 0x96), 0, 0xa, 0x100, 0x50);

    idx = *(int *)(self + 0x474) - 1;
    if ((int)idx < 0)
        idx = *(int *)(self + 0x470) - 1;
    path.GetNode(node2, idx);
    Vec3_Sub(&diff, (Vector3 *)(self + 0x5c), &node);
    len = LenVec3(&diff);
    if (len == 0)
        goto arrived;
    if (len > 0x14000)
        goto faraway;

arrived:
    *(int *)(self + 0x460) = 7;
    if (*(u8 *)(self + 0x448) == 2 || (u8)(s8)(LEVEL_ID - 0x18) <= 1) {
        (*(int *)(((int)self + 0x474) & 0xFFFFFFFFFFFFFFFF))++;
        if (*(int *)(self + 0x474) >= 4)
            *(int *)(self + 0x474) = 0;
    } else if (func_ov062_0211b3ac(self) == 0) {
        u32 r = (u32)RandomIntInternal(&RNG_STATE) >> 8 & 3;
        if (*(int *)(self + 0x474) != r) {
            *(int *)(self + 0x474) = r;
        } else {
            int *p = (int *)(((int)self + 0x474) & 0xFFFFFFFFFFFFFFFF);
            (*p)++;
            *p &= 3;
        }
    }
    *(int *)(self + 0x5c) = node.x; *(int *)(self + 0x60) = node.y; *(int *)(self + 0x64) = node.z;
    *(int *)(self + 0x430) = *(int *)(self + 0x5c);
    *(int *)(self + 0x434) = *(int *)(self + 0x60);
    *(int *)(self + 0x438) = *(int *)(self + 0x64);
    zero[2] = 0; zero[0] = 0; zero[1] = 0; zero[2] = 0x14000;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(s16 *)(self + 0x8e));
    MulVec3Mat4x3((Vector3 *)zero, &MATRIX_SCRATCH_PAPER, self + 0xa4);
    func_ov062_0211c658(self, &data_ov062_0211e17c);
    return 1;

faraway:
    if (AngleDiff(Vec3_HorzAngle(self + 0x5c, &node), *(s16 *)(self + 0x8e)) >= 0x2000)
        goto ret1;
    {
        int s;
        zero[1] = *(int *)(self + 0x60);
        s = _ZN4cstd4fdivEii(0x14000, len);
        Vec3_MulScalar(&scaled, &diff, s);
        SubVec3(self + 0x5c, &scaled, self + 0x5c);
        if (*(u8 *)(self + 0x446) != 1)
            goto ret1;
        *(int *)(self + 0x60) = zero[1];
        if (*(u16 *)(self + 0x100) == 0) {
            *(int *)(self + 0xa8) = 0;
            *(u8 *)(self + 0x446) = 0;
        }
    }
ret1:
    return 1;
}
