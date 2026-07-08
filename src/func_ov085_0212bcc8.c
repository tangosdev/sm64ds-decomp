struct Vec3 { int x, y, z; };
struct Mtx { int w[12]; };

extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void* a, void* b, void* dst);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* thiz, void* sm, void* mtx, int f, int a, unsigned int b);

extern struct Mtx MATRIX_SCRATCH_PAPER;

void func_ov085_0212bcc8(char* c)
{
    char tmp[0x30];
    struct Vec3 t;
    Vec3_Asr(&t, (struct Vec3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER,
        *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(struct Mtx*)(c + 0x31c) = MATRIX_SCRATCH_PAPER;
    MulMat4x3Mat4x3(*(void**)(c + 0x314), (void*)(c + 0x31c), tmp);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER,
        *(int*)(c + 0x5c) >> 3,
        (*(int*)(c + 0x60) - 0xe000) >> 3,
        *(int*)(c + 0x64) >> 3);
    *(struct Mtx*)(c + 0x390) = MATRIX_SCRATCH_PAPER;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (void*)(c + 0x368), (void*)(c + 0x390), 0x46000, 0x258000, 0xf);
}
