struct Vec3 { int x, y, z; };
struct Matrix4x3 { int m[12]; };

extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_Lsl(struct Vec3 *d, struct Vec3 *s, int sh);
extern unsigned int _ZN5Actor10FindWithIDEj(unsigned int id);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;

void func_ov078_02125de0(char *c)
{
    struct Vec3 lv;
    int i;

    Matrix4x3_FromRotationY(c + 0x2e8, *(short*)(c + 0x8e));
    *(int*)(c + 0x30c) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x310) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x314) = *(int*)(c + 0x64) >> 3;
    MulMat4x3Mat4x3(*(char**)(c + 0x2e0) + 0x1e0, c + 0x2e8, c + 0x4a4);

    *(void**)(c + 0xc8) = c + 0x4a4;
    *(int*)(c + 0x4ec) = 0;
    *(int*)(c + 0x4f0) = 0;
    *(int*)(c + 0x4f4) = 0;
    *(int*)(c + 0x4ec) = *(int*)(c + 0x4c8);
    *(int*)(c + 0x4f0) = *(int*)(c + 0x4cc);
    *(int*)(c + 0x4f4) = *(int*)(c + 0x4d0);
    Vec3_Lsl(&lv, (struct Vec3*)(c + 0x4ec), 3);
    *(int*)(c + 0x4ec) = lv.x;
    *(int*)(c + 0x4f0) = lv.y;
    *(int*)(c + 0x4f4) = lv.z;

    for (i = 0; i < 2; i++) {
        unsigned int id;
        void *actor;

        if (((unsigned char*)(c + 0x42c))[i] != 0)
            continue;
        id = ((unsigned int*)(c + 0x424))[i];
        if (id == 0)
            continue;
        actor = (void*)_ZN5Actor10FindWithIDEj(id);
        if (actor == 0)
            continue;

        MATRIX_SCRATCH_PAPER = *(struct Matrix4x3*)(c + 0x4a4);
        Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0xb000, -0x7000, -0x6000);
        Matrix4x3_ApplyInPlaceToRotationXYZExt(&MATRIX_SCRATCH_PAPER, 0x4000, -0x1000, 0x2000);
        *(struct Matrix4x3*)(c + 0x4a4) = MATRIX_SCRATCH_PAPER;
        *(void**)((char*)actor + 0xc8) = c + 0x4a4;
    }
}
