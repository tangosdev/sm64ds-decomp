//cpp
struct Matrix4x3 { int m[12]; };

struct Obj { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); };

extern "C" {
void Vec3_Asr(int *d, int *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
unsigned int _ZN5Actor10FindWithIDEj(unsigned int id);
void MulMat4x3Mat4x3(void *dst, void *a, void *b);
void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *mtx, int a, int b, unsigned int g);
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;
}

extern "C" void func_ov081_021254d8(char *c)
{
    int src[3], dst[3];
    int t;
    void *actor;

    ((Obj*)(c + 0x30c))->v3();

    src[0] = *(int*)(c + 0x5c);
    t = *(int*)(c + 0x60);
    src[1] = t;
    src[2] = *(int*)(c + 0x64);
    src[1] = t + *(int*)(c + 0x45c);
    Vec3_Asr(dst, src, 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, dst[0], dst[1], dst[2]);

    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER,
        *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(struct Matrix4x3*)(c + 0x328) = MATRIX_SCRATCH_PAPER;

    if (*(unsigned int*)(c + 0x400) != 0) {
        *(int*)(c + 0x440) = 0;
        *(int*)(c + 0x444) = 0;
        *(int*)(c + 0x448) = 0;
        actor = (void*)_ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x400));
        if (actor != 0) {
            MulMat4x3Mat4x3(*(char**)(c + 0x320) + 0xf0, c + 0x328, c + 0x3c8);

            *(int*)(c + 0x440) = MATRIX_SCRATCH_PAPER.m[9];
            *(int*)(c + 0x444) = MATRIX_SCRATCH_PAPER.m[10];
            *(int*)(c + 0x448) = MATRIX_SCRATCH_PAPER.m[11];
            *(int*)(((int)c + 0x440) & 0xFFFFFFFFFFFFFFFF) <<= 3;
            *(int*)(((int)c + 0x444) & 0xFFFFFFFFFFFFFFFF) <<= 3;
            *(int*)(((int)c + 0x448) & 0xFFFFFFFFFFFFFFFF) <<= 3;
            *(int*)((char*)actor + 0x5c) = *(int*)(c + 0x440);
            *(int*)((char*)actor + 0x60) = *(int*)(c + 0x444);
            *(int*)((char*)actor + 0x64) = *(int*)(c + 0x448);

            Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, 0, 0x4000, 0);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER, 0, 0x2000, 0);
            MulMat4x3Mat4x3(&MATRIX_SCRATCH_PAPER, c + 0x3c8, c + 0x3c8);
            *(void**)((char*)actor + 0xc8) = c + 0x3c8;
        }
    }

    *(int*)(c + 0x434) = 0;
    *(int*)(c + 0x438) = 0;
    *(int*)(c + 0x43c) = 0;
    MATRIX_SCRATCH_PAPER = *(struct Matrix4x3*)(c + 0x328);
    MulMat4x3Mat4x3(*(char**)(c + 0x320) + 0x90, &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);
    Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0xc000, -0x1000, 0);

    *(int*)(c + 0x434) = MATRIX_SCRATCH_PAPER.m[9];
    *(int*)(c + 0x438) = MATRIX_SCRATCH_PAPER.m[10];
    *(int*)(c + 0x43c) = MATRIX_SCRATCH_PAPER.m[11];
    *(int*)(((int)c + 0x434) & 0xFFFFFFFFFFFFFFFF) <<= 3;
    *(int*)(((int)c + 0x438) & 0xFFFFFFFFFFFFFFFF) <<= 3;
    *(int*)(((int)c + 0x43c) & 0xFFFFFFFFFFFFFFFF) <<= 3;

    if (*(int*)(c + 0x41c) != 0)
        return;

    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER,
        *(int*)(c + 0x5c) >> 3,
        (*(int*)(c + 0x60) - 0xe000) >> 3,
        *(int*)(c + 0x64) >> 3);
    *(struct Matrix4x3*)(c + 0x398) = MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x370, c + 0x398, 0x78000, 0xc8000, 0xf);
}
