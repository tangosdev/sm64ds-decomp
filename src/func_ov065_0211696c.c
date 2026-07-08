// NONMATCHING: base materialization / addressing (div=17). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
typedef short s16;

typedef struct Mtx43 { int w[12]; } Mtx43;

extern void Vec3_Asr(void* d, void* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void* dst, void* a, void* b);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* c, void* shadow, void* mtx, int rad, int height, u32 flags);

extern Mtx43 MATRIX_SCRATCH_PAPER;

struct V3 { int x, y, z; };

void func_ov065_0211696c(char* c)
{
    struct V3 v;
    Vec3_Asr(&v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER,
        *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));

    *(Mtx43*)(c + 0x31c) = MATRIX_SCRATCH_PAPER;
    *(int*)(c + 0x3c0) = 0;
    *(int*)(c + 0x3c4) = 0;
    *(int*)(c + 0x3c8) = 0;
    MATRIX_SCRATCH_PAPER = *(Mtx43*)(c + 0x31c);

    MulMat4x3Mat4x3(*(char**)(c + 0x314) + 0xc0, &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);

    *(int*)(c + 0x3c0) = *(int*)((char*)&MATRIX_SCRATCH_PAPER + 0x24);
    *(int*)(c + 0x3c4) = *(int*)((char*)&MATRIX_SCRATCH_PAPER + 0x28);
    *(int*)(c + 0x3c8) = *(int*)((char*)&MATRIX_SCRATCH_PAPER + 0x2c);
    *(int*)(c + 0x3c0) <<= 3;
    *(int*)(c + 0x3c4) <<= 3;
    *(int*)(c + 0x3c4) -= 0xa000;
    *(int*)(c + 0x3c8) <<= 3;

    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER,
        *(int*)(c + 0x5c) >> 3,
        (*(int*)(c + 0x60) - 0x18000) >> 3,
        *(int*)(c + 0x64) >> 3);

    *(Mtx43*)(c + 0x38c) = MATRIX_SCRATCH_PAPER;

    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x364, c + 0x38c, 0x64000, 0x258000, 0xf);
}
