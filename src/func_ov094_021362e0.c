// NONMATCHING: different op / idiom (div=35). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12i;
typedef short s16;

struct Vec3 { Fix12i x, y, z; };
struct Mtx43 { Fix12i a[12]; };

extern struct Mtx43 MATRIX_SCRATCH_PAPER;
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(struct Mtx43* m, int x, int y, int z);

void func_ov094_021362e0(char* c)
{
    struct Vec3 v;
    char* m;
    if (*(int*)(c + 0x3cc) == 0) return;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    m = *(char**)(c + 0x3cc);
    MATRIX_SCRATCH_PAPER = *(struct Mtx43*)(*(char**)(m + 0xc8));
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0x3000, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER, -0x6000, 0x1000, 0x4000);
    v.x = MATRIX_SCRATCH_PAPER.a[9];
    v.y = MATRIX_SCRATCH_PAPER.a[10];
    v.z = MATRIX_SCRATCH_PAPER.a[11];
    *(int*)(c + 0x5c) = v.x << 3;
    *(int*)(c + 0x60) = v.y << 3;
    *(int*)(c + 0x64) = v.z << 3;
    *(s16*)(c + 0x94) = *(s16*)(*(char**)(c + 0x3cc) + 0x8e);
    *(s16*)(c + 0x92) = 0;
    *(struct Mtx43*)(c + 0x328) = MATRIX_SCRATCH_PAPER;
}
